/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "foreach.h"
#include "codegen.h"
#include "parser.hpp"

namespace mlang {

    llvm::Value *ForEach::codeGen(CodeGenContext &context) {
        llvm::Function *function = context.currentBlock()->getParent();

        llvm::BasicBlock *beforeBB = llvm::BasicBlock::Create(context.getGlobalContext(), "before", function);
        llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(context.getGlobalContext(), "loop");
        llvm::BasicBlock *progressBB = llvm::BasicBlock::Create(context.getGlobalContext(), "progress");
        llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(context.getGlobalContext(), "after");

        llvm::Value *array = expr->codeGen(context);

        if (array == nullptr || !array->getType()->isPointerTy()) {
            Node::printError(location, "Invalid value in foreach loop");
            context.addError();
            return nullptr;
        }

        llvm::Value *from = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 0);
        llvm::Value *to = context.callSizeOf(array);
        llvm::Value *stepVal = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 1);

        if (to == nullptr) {
            Node::printError(location, "Error in foreach loop definition.");
            context.addError();
            return nullptr;
        }

        llvm::BranchInst::Create(beforeBB, context.currentBlock());
        context.newScope(beforeBB, ScopeType::CODE_BLOCK);

        auto *alloc = new llvm::AllocaInst(array->getType()->getPointerElementType(), 0, ident->getName(),
                                           context.currentBlock());
        auto variable = Variable::newLocal(alloc);
        context.locals()[ident->getName()] = variable;
        context.setVarType(context.llvmTypeToString(array->getType()), ident->getName());
        
        auto *indexAlloc = new llvm::AllocaInst(llvm::Type::getInt64Ty(context.getGlobalContext()), 0, "index",
                                                context.currentBlock());
        auto index = Variable::newLocal(indexAlloc);
        new llvm::StoreInst(from, index->getValue(), false, context.currentBlock());

        llvm::Value *indices[1] = {from};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, array, indices, "elem_ptr",
                                                                  context.currentBlock());
        auto item = new llvm::LoadInst(array->getType()->getPointerElementType(), elementPtr, "item", context.currentBlock());
        new llvm::StoreInst(item, variable->getValue(), false, context.currentBlock());

        auto op = llvm::CmpInst::ICMP_SLT;
        auto cmp = llvm::CmpInst::Create(llvm::Instruction::ICmp, op, from, to, "cmptmp", context.currentBlock());
        llvm::BranchInst::Create(loopBB, afterBB, cmp, context.currentBlock());

        function->getBasicBlockList().push_back(loopBB);

        context.newScope(loopBB, ScopeType::CODE_BLOCK, afterBB);

        llvm::Value *loopValue = this->doBlock->codeGen(context);
        if (loopValue == nullptr || !mlang::CodeGenContext::isBreakingInstruction(loopValue)) {
            llvm::BranchInst::Create(progressBB, context.currentBlock());
        }
        function->getBasicBlockList().push_back(progressBB);
        context.endScope();

        context.newScope(progressBB, ScopeType::CODE_BLOCK);
        llvm::Value *indexValue = new llvm::LoadInst(index->getType(), index->getValue(), "index", false,
                                                context.currentBlock());
        auto tmp = llvm::BinaryOperator::Create(llvm::Instruction::Add, indexValue, stepVal, "mathtmp",
                                                context.currentBlock());
        new llvm::StoreInst(tmp, index->getValue(), false, context.currentBlock());

        llvm::Value *indices2[1] = {tmp};
        elementPtr = llvm::GetElementPtrInst::Create(nullptr, array, indices2, "elem_ptr",
                                                                  context.currentBlock());
        item = new llvm::LoadInst(array->getType()->getPointerElementType(), elementPtr, "item", context.currentBlock());
        new llvm::StoreInst(item, variable->getValue(), false, context.currentBlock());

        cmp = llvm::CmpInst::Create(llvm::Instruction::ICmp, op, tmp, to, "cmptmp", context.currentBlock());
        llvm::BranchInst::Create(loopBB, afterBB, cmp, context.currentBlock());
        context.endScope();

        context.endScope();
        function->getBasicBlockList().push_back(afterBB);
        context.setInsertPoint(afterBB);

        return afterBB;
    }

}