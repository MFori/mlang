/**
 * Created by Martin on 12.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "break.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Break::codeGen(CodeGenContext &context) {
        llvm::BasicBlock *exitBB = context.getExitBlockFromCurrent();

        if (exitBB == nullptr) {
            Node::printError(location, "Cannot call 'break' here.");
            context.addError();
            return nullptr;
        }

        return llvm::BranchInst::Create(exitBB, context.currentBlock());
    }

}