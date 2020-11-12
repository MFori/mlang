/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "forloop.h"
#include "codegen.h"
#include "variable.h"
#include "binaryop.h"
#include "parser.hpp"

namespace mlang {

    llvm::Value *ForLoop::codeGen(CodeGenContext &context) {
        llvm::Function *function = context.currentBlock()->getParent();

        llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(context.getGlobalContext(), "loop");
        llvm::BasicBlock *progressBB = llvm::BasicBlock::Create(context.getGlobalContext(), "progress");
        llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(context.getGlobalContext(), "after");

        llvm::Value *from = range->getLhs()->codeGen(context);
        llvm::Value *to = range->getRhs()->codeGen(context);
        llvm::Value *stepVal = step != nullptr ? step->codeGen(context) : llvm::ConstantInt::get(
                llvm::Type::getInt64Ty(context.getGlobalContext()), 1);

        if (from == nullptr || to == nullptr || stepVal == nullptr) {
            Node::printError(location, "Error in for loop definition.");
            context.addError();
            return nullptr;
        }

        if (!from->getType()->isIntegerTy() || !to->getType()->isIntegerTy() || !stepVal->getType()->isIntegerTy()) {
            Node::printError(location, "Only integers can be used to control for loop.");
            context.addError();
            return nullptr;
        }

        auto *alloc = new llvm::AllocaInst(llvm::Type::getInt64Ty(context.getGlobalContext()), 0, ident->getName(),
                                           context.currentBlock());
        auto variable = Variable::newLocal(alloc);
        context.locals()[ident->getName()] = variable;
        context.setVarType("Int", ident->getName());
        new llvm::StoreInst(from, variable->getValue(), false, context.currentBlock());

        llvm::BranchInst::Create(loopBB, context.currentBlock());
        function->getBasicBlockList().push_back(loopBB);

        context.newScope(loopBB, ScopeType::CODE_BLOCK, afterBB);

        ((llvm::AllocaInst *) variable->getValue())->getType()->getElementType();
        llvm::Value *loopValue = this->doBlock->codeGen(context);
        if (loopValue == nullptr || !mlang::CodeGenContext::isBreakingInstruction(loopValue)) {
            llvm::BranchInst::Create(progressBB, context.currentBlock());
        }
        function->getBasicBlockList().push_back(progressBB);
        context.endScope();

        context.newScope(progressBB, ScopeType::CODE_BLOCK);
        llvm::Value *value = new llvm::LoadInst(variable->getType(), variable->getValue(), ident->getName(), false,
                                                context.currentBlock());
        auto tmp = llvm::BinaryOperator::Create(llvm::Instruction::Add, value, stepVal, "mathtmp",
                                                context.currentBlock());
        new llvm::StoreInst(tmp, variable->getValue(), false, context.currentBlock());
        auto op = range->getOp() == TUNTIL ? llvm::CmpInst::ICMP_SLT : llvm::CmpInst::ICMP_SLE;
        auto cmp = llvm::CmpInst::Create(llvm::Instruction::ICmp, op, tmp, to, "cmptmp", context.currentBlock());
        llvm::BranchInst::Create(loopBB, afterBB, cmp, context.currentBlock());
        function->getBasicBlockList().push_back(afterBB);
        context.endScope();

        context.setInsertPoint(afterBB);
        return afterBB;
    }

}