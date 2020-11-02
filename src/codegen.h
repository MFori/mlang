/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_CODEGEN_H
#define MLANG_CODEGEN_H

#include <list>

#pragma warning(push, 0)

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#pragma warning(pop)

#include "ast.h"

namespace mlang {
    class CodeGenBlock {
    public:
        llvm::BasicBlock *currentBlock() { return block; }

    private:
        llvm::BasicBlock *block{nullptr};
    };

    class CodeGenContext {
    public:
        explicit CodeGenContext(std::ostream &outs);

        llvm::LLVMContext &getGlobalContext() { return llvmContext; }

        llvm::BasicBlock *currentBlock() { return codeBlocks.front()->currentBlock(); }

        bool generateCode(class Block &root);

        llvm::GenericValue runCode();

        bool preProcessing(class Block &root);

    private:
        std::list<CodeGenBlock *> codeBlocks;
        llvm::LLVMContext llvmContext;
    };
}

#endif /* MLANG_CODEGEN_H */
