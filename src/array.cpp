/**
 * Created by Martin on 14.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "array.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Array::codeGen(CodeGenContext &context) {
        llvm::Value *count = size->codeGen(context);

        if (count == nullptr || !count->getType()->isIntegerTy()) {
            Node::printError(location, "Invalid array size");
            context.addError();
            return nullptr;
        }

        auto intSize = llvm::ConstantExpr::getSizeOf(llvm::Type::getInt64Ty(context.getGlobalContext()));
        auto array = context.createMallocCall(type, count, "array", intSize);

        llvm::Value *indices[1] = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 1)};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, array, indices, "elem_ptr",
                                                                  context.currentBlock());

        new llvm::StoreInst(count, array, false, context.currentBlock());
        //return elementPtr;
        return new llvm::BitCastInst(elementPtr, type->getPointerTo(), "array", context.currentBlock());
    }

    void Array::validateArrayBounds(llvm::Value *arrayPtr, llvm::Value *index, CodeGenContext &context) {
        llvm::Function *function = context.currentBlock()->getParent();
        llvm::BasicBlock *validateBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "arr_validate",
                                                                   function);
        llvm::BasicBlock *errBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "arr_err");
        llvm::BasicBlock *successBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "arr_success");

        auto zeroVal = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 0);
        auto cmp = llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLT, index, zeroVal, "cmptmp",
                                         context.currentBlock());
        llvm::BranchInst::Create(errBlock, validateBlock, cmp, context.currentBlock());

        context.newScope(validateBlock, ScopeType::CODE_BLOCK);
        llvm::Value *arraySize = context.callSizeOf(arrayPtr);

        cmp = llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLT, index, arraySize, "cmptmp",
                                    context.currentBlock());
        llvm::BranchInst::Create(successBlock, errBlock, cmp, context.currentBlock());

        function->getBasicBlockList().push_back(errBlock);
        context.endScope();

        context.newScope(errBlock, ScopeType::CODE_BLOCK);

        context.runtimeError(RuntimeError::INDEX_OUT_OF_RANGE);
        llvm::BranchInst::Create(successBlock, context.currentBlock());

        function->getBasicBlockList().push_back(successBlock);
        context.endScope();

        context.newScope(successBlock, ScopeType::CODE_BLOCK);
        context.endScope();
        context.setInsertPoint(successBlock);
    }

    llvm::Value *ArrayAccess::codeGen(CodeGenContext &context) {
        llvm::Value *indexValue = index->codeGen(context);
        llvm::Value *var = ident->codeGen(context);

        if (indexValue == nullptr || !indexValue->getType()->isIntegerTy()) {
            Node::printError(location, "Invalid index value");
            context.addError();
            return nullptr;
        }

        if (var == nullptr || !var->getType()->isPointerTy()) {
            Node::printError(location, "variable '" + ident->getName() + "' is not array");
            context.addError();
            return nullptr;
        }

        Array::validateArrayBounds(var, indexValue, context);

        llvm::Value *indices[1] = {indexValue};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, var, indices, "elem_ptr",
                                                                  context.currentBlock());

        return new llvm::LoadInst(var->getType()->getPointerElementType(), elementPtr, "item", context.currentBlock());
    }

    llvm::Value *ArrayAssignment::codeGen(CodeGenContext &context) {
        llvm::Value *indexValue = index->codeGen(context);
        llvm::Value *var = ident->codeGen(context);
        llvm::Value *value = rhs->codeGen(context);

        if (indexValue == nullptr || !indexValue->getType()->isIntegerTy()) {
            Node::printError(location, "Invalid index value");
            context.addError();
            return nullptr;
        }

        if (var == nullptr || !var->getType()->isPointerTy()) {
            Node::printError(location, "variable '" + ident->getName() + "' is not array");
            context.addError();
            return nullptr;
        }

        if (value == nullptr || value->getType() != var->getType()->getPointerElementType()) {
            Node::printError(location, "invalid assignment to array");
            context.addError();
            return nullptr;
        }

        Array::validateArrayBounds(var, indexValue, context);

        llvm::Value *indices[1] = {indexValue};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, var, indices, "elem_ptr",
                                                                  context.currentBlock());

        new llvm::StoreInst(value, elementPtr, false, context.currentBlock());
        return value;
    }
}