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

        // TODO count validation can be only integer

        // TODO store size at front

        return context.createMallocCall(llvm::Type::getInt64Ty(context.getGlobalContext()), count, "array");

        // TODO create free construct
    }

    llvm::Value *ArrayAccess::codeGen(CodeGenContext &context) {
        llvm::Value *indexValue = index->codeGen(context);
        llvm::Value *var = ident->codeGen(context);

        // TODO validation
        // TODO validate bounds

        llvm::Value *indices[1] = {indexValue};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, var, indices, "elem_ptr",
                                                                  context.currentBlock());

        return new llvm::LoadInst(llvm::Type::getInt64Ty(context.getGlobalContext()), elementPtr, "item",
                                  context.currentBlock());
    }

    llvm::Value *ArrayAssignment::codeGen(CodeGenContext &context) {
        llvm::Value *indexValue = index->codeGen(context);
        llvm::Value *var = ident->codeGen(context);
        llvm::Value *value = rhs->codeGen(context);

        // TODO validation
        // TODO validate bounds

        llvm::Value *indices[1] = {indexValue};
        llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, var, indices, "elem_ptr",
                                                                  context.currentBlock());

        new llvm::StoreInst(value, elementPtr, false, context.currentBlock());
        return value;
    }
}