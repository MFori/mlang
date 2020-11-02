/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "return.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Return::codeGen(CodeGenContext &context) {
        if(returnExpression) {
            llvm::Value* ret = returnExpression->codeGen(context);
            if(ret == nullptr) {
                return nullptr;
            }
            return llvm::ReturnInst::Create(context.getGlobalContext(), ret, context.currentBlock());
        } else {
            return llvm::ReturnInst::Create(context.getGlobalContext(), nullptr, context.currentBlock());
        }
    }

}