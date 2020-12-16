/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "conditional.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Conditional::codeGen(CodeGenContext &context) {
        llvm::Value *comp = condExpr->codeGen(context);
        if (comp == nullptr) {
            Node::printError(location, "Code generation for compare operator of the conditional statement failed.");
            context.addError();
            return nullptr;
        }
        if (!comp->getType()->isIntegerTy(1)) {
            Node::printError(location, "If condition doesn't result in a boolean expression.");
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
        auto thenVal = thenExpr->codeGen(context);

        if (thenVal == nullptr || !mlang::CodeGenContext::isBreakingInstruction(thenVal)) {
            llvm::BranchInst::Create(mergeBlock, context.currentBlock());
            needMergeBlock = true;
        }

        function->getBasicBlockList().push_back(elseBlock);
        context.endScope();

        context.newScope(elseBlock, ScopeType::CODE_BLOCK);
        llvm::Value *elseVal = nullptr;
        if (elseExpr != nullptr) {
            elseVal = elseExpr->codeGen(context);
        }

        if (elseVal == nullptr || !mlang::CodeGenContext::isBreakingInstruction(elseVal)) {
            llvm::BranchInst::Create(mergeBlock, context.currentBlock());
            needMergeBlock = true;
        }
        context.endScope();

        if (needMergeBlock) {
            function->getBasicBlockList().push_back(mergeBlock);
            context.setInsertPoint(mergeBlock);
        }

        if (!needMergeBlock) {
            return thenVal;
        }

        return mergeBlock;
    }

}
