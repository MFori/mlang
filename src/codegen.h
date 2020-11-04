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
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>

/*#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/ManagedStatic.h>*/

#pragma warning(pop)

#include "ast.h"

namespace mlang {

    enum class ScopeType {
        FUNCTION_DECL,
        CODE_BLOCK
    };

    using ValueNames = std::map<std::string, llvm::AllocaInst *>;
    using VariableTypeMap = std::map<std::string, std::string>;


    class CodeGenBlock {
    public:
        explicit CodeGenBlock(llvm::BasicBlock *bb) { block = bb; }

        ~CodeGenBlock() = default;

        llvm::BasicBlock *currentBlock() { return block; }

        ValueNames &getValueNames() { return locals; }

        VariableTypeMap &getTypeMap() { return types; }

    private:
        llvm::BasicBlock *block{nullptr};
        ValueNames locals;
        VariableTypeMap types;
    };

    class CodeGenContext {
    public:
        explicit CodeGenContext(std::ostream &outs);

        ~CodeGenContext() { llvm::llvm_shutdown(); }

        llvm::LLVMContext &getGlobalContext() { return llvmContext; }

        llvm::Module *getModule() const { return module; }

        void newScope(llvm::BasicBlock *bb = nullptr, ScopeType sc = ScopeType::CODE_BLOCK);

        void endScope();

        ScopeType getScopeType() { return scopeType; }

        llvm::BasicBlock *currentBlock() { return codeBlocks.front()->currentBlock(); }

        llvm::AllocaInst *findVariable(const std::string& name);

        ValueNames &locals() { return codeBlocks.front()->getValueNames(); }

        void optimize();

        bool generateCode(class Block &root);

        llvm::GenericValue runCode();

        bool preProcessing(class Block &root);

        void setUpBuildIns();

        std::string getType(std::string varName);

        void addError() { ++errors; }

    private:
        std::list<CodeGenBlock *> codeBlocks;
        llvm::Function *mainFunction{nullptr};
        llvm::Module *module{nullptr};
        llvm::LLVMContext llvmContext;
        ScopeType scopeType{ScopeType::CODE_BLOCK};
        std::ostream &outs;
        int errors{0};

        struct buildin_info_t {
            llvm::Function *f{nullptr};
            void *addr{nullptr};
        };
        std::vector<buildin_info_t> buildins;
        llvm::Type *intType{nullptr};
    };
}

#endif /* MLANG_CODEGEN_H */
