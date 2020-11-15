/**
 * Created by Martin on 15.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "free.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *FreeMemory::codeGen(CodeGenContext &context) {
        llvm::Value *ptrValue = ptr->codeGen(context);

        if (ptrValue == nullptr) {
            Node::printError(location, "invalid rm usage");
            context.addError();
            return nullptr;
        }

        if (!ptrValue->getType()->isPointerTy()) {
            Node::printError(location, "rm can be used only for arrays and dynamically created strings");
            context.addError();
            return nullptr;
        }

        context.createFreeCall(ptrValue);
        return nullptr;
    }

}