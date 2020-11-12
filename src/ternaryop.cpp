/**
 * Created by Martin on 12.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "ternaryop.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *TernaryOp::codeGen(CodeGenContext &context) {
        llvm::Value *comp = condExpr->codeGen(context);
        if (comp == nullptr) {
            Node::printError(location, "Code generation for compare operator of the conditional statement failed.");
            context.addError();
            return nullptr;
        }
        if (!comp->getType()->isIntegerTy(1)) {
            Node::printError(location, "Ternary condition doesn't result in a boolean expression.");
            context.addError();
            return nullptr;
        }

        llvm::Function *function = context.currentBlock()->getParent();
        llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "then", function);
        llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "else");
        llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(context.getGlobalContext(), "merge");

        llvm::BranchInst::Create(thenBlock, elseBlock, comp, context.currentBlock());

        bool needMergeBlock = false;

        context.newScope(thenBlock, ScopeType::CODE_BLOCK);
        auto val1 = thenExpr->codeGen(context);

        if (context.currentBlock()->getTerminator() == nullptr) {
            llvm::BranchInst::Create(mergeBlock, context.currentBlock());
            needMergeBlock = true;
        }

        function->getBasicBlockList().push_back(elseBlock);
        context.endScope();

        context.newScope(elseBlock, ScopeType::CODE_BLOCK);
        auto val2 = elseExpr->codeGen(context);

        if (context.currentBlock()->getTerminator() == nullptr) {
            llvm::BranchInst::Create(mergeBlock, context.currentBlock());
            needMergeBlock = true;
        }
        context.endScope();

        if (needMergeBlock) {
            function->getBasicBlockList().push_back(mergeBlock);
            context.setInsertPoint(mergeBlock);
        }

        llvm::Type *ty = val1->getType();
        if (ty != val2->getType()) {
            Node::printError(location, "Ternary operator different types.");
            context.addError();
            return nullptr;
        }

        llvm::PHINode *PN = llvm::PHINode::Create(ty, 2, "iftmp", context.currentBlock());

        PN->addIncoming(val1, thenBlock);
        PN->addIncoming(val2, elseBlock);

        return PN;
    }

}