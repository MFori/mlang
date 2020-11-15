/**
 * Created by Martin on 14.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "codegen.h"

namespace mlang {

    llvm::Value *Array::codeGen(CodeGenContext &context) {
        llvm::Value *count = size->codeGen(context);

        if(count == nullptr || !count->getType()->isIntegerTy()) {
            Node::printError(location, "Invalid array size");
            context.addError();
            return nullptr;
        }

        // TODO count validation can be only integer

        // TODO store size at front
        // TODO create get length function

        return context.createMallocCall(type, count, "array");
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

        // TODO validate bounds

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

        // TODO validate bounds

        llvm::Value *indices[1] = {indexValue};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, var, indices, "elem_ptr",
                                                                  context.currentBlock());

        new llvm::StoreInst(value, elementPtr, false, context.currentBlock());
        return value;
    }
}