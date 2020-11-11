/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "whileloop.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *WhileLoop::codeGen(CodeGenContext &context) {
        llvm::Function *function = context.currentBlock()->getParent();

        llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context.getGlobalContext(), "cond", function);
        llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(context.getGlobalContext(), "loop");
        llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(context.getGlobalContext(), "after");

        llvm::BranchInst::Create(doFirst ? loopBB : condBB, context.currentBlock());
        context.newScope(condBB, ScopeType::CODE_BLOCK);

        llvm::Value *condValue = this->condition->codeGen(context);
        if (condValue == nullptr) {
            Node::printError(location, "Code gen for condition expression in while loop failed.");
            context.addError();
            return nullptr;
        }
        if (!condValue->getType()->isIntegerTy(1)) {
            Node::printError(location, "While condition doesn't result in a boolean expression.");
            context.addError();
            return nullptr;
        }
        llvm::BranchInst::Create(loopBB, afterBB, condValue, context.currentBlock());

        function->getBasicBlockList().push_back(loopBB);
        context.endScope();
        context.newScope(loopBB, ScopeType::CODE_BLOCK);
        llvm::Value *loopValue = this->doBlock->codeGen(context);
        if (loopValue == nullptr) {
            Node::printError(location, "Code gen for loop value in while loop failed.");
            context.addError();
            return nullptr;
        }
        llvm::BranchInst::Create(condBB, context.currentBlock());

        function->getBasicBlockList().push_back(afterBB);
        context.endScope();
        context.setInsertPoint(afterBB);
        return afterBB;
    }

}