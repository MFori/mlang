/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_CODEGEN_H
#define MLANG_CODEGEN_H

#include <list>
#include <utility>

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
#include <llvm/IR/IRBuilder.h>

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

#include "buildins.h"
#include "ast.h"
#include "array.h"
#include "cast.h"

namespace mlang {

    enum class ScopeType {
        FUNCTION_DECL,
        CODE_BLOCK,
        GLOBAL_BLOCK
    };

    enum class VariableScope {
        GLOBAL,
        LOCAL
    };

    class Variable {
    public:
        Variable(llvm::Value *value, VariableScope type, bool constant) : value(value), type(type),
                                                                          constant(constant) {}

        static Variable *newLocal(llvm::AllocaInst *allocaInst, bool constant = false) {
            return new Variable(allocaInst, VariableScope::LOCAL, constant);
        }

        static Variable *newGlobal(llvm::GlobalVariable *global, bool constant = false) {
            return new Variable(global, VariableScope::GLOBAL, constant);
        }

        llvm::Value *getValue() { return value; }

        void setValue(llvm::Value *val) { value = val; }

        llvm::Type *getType();

        VariableScope getVariableScope() { return type; }

        bool isLocal() { return type == VariableScope::LOCAL; }

        bool isGlobal() { return type == VariableScope::GLOBAL; }

        bool isConst() const { return constant; }

    private:
        llvm::Value *value{nullptr};
        VariableScope type{VariableScope::LOCAL};
        bool constant{false};
    };

    using ValueNames = std::map<std::string, Variable *>;
    using VariableTypeMap = std::map<std::string, std::string>;

    class CodeGenBlock {
    public:
        explicit CodeGenBlock(llvm::BasicBlock *bb, ScopeType sc, llvm::BasicBlock *exitBB = nullptr) {
            block = bb;
            scopeType = sc;
            exitBlock = exitBB;
        }

        ~CodeGenBlock() = default;

        void setCodeBlock(llvm::BasicBlock *bb) { block = bb; }

        llvm::BasicBlock *currentBlock() { return block; }

        ValueNames &getValueNames() { return locals; }

        VariableTypeMap &getTypeMap() { return types; }

        ScopeType getScopeType() { return scopeType; }

        llvm::BasicBlock *getExitBlock() { return exitBlock; }

    private:
        llvm::BasicBlock *block{nullptr};
        llvm::BasicBlock *exitBlock{nullptr};
        ScopeType scopeType;
        ValueNames locals;
        VariableTypeMap types;
    };

    class CodeGenContext {
    public:
        explicit CodeGenContext(std::ostream &outs, bool debug);

        ~CodeGenContext() { llvm::llvm_shutdown(); }

        llvm::LLVMContext &getGlobalContext() { return llvmContext; }

        llvm::Module *getModule() const { return module; }

        void newScope(llvm::BasicBlock *bb, ScopeType sc, llvm::BasicBlock *exitBB = nullptr);

        void endScope();

        ScopeType getScopeType() { return scopeType; }

        llvm::BasicBlock *currentBlock() { return codeBlocks.front()->currentBlock(); }

        llvm::BasicBlock *getExitBlockFromCurrent();

        void setInsertPoint(llvm::BasicBlock *bblock) { setCurrentBlock(bblock); }

        Variable *findVariable(const std::string &name, bool onlyLocals = true);

        bool hasVariable(const std::string &name);

        ValueNames &locals() { return codeBlocks.front()->getValueNames(); }

        llvm::Type *typeOf(const class Identifier &type);

        llvm::Type *typeOf(const std::string &name);

        void setVarType(std::string varTypeName, const std::string &varName) {
            codeBlocks.front()->getTypeMap()[varName] = std::move(varTypeName);
        }

        std::string getVarType(const std::string &varName);

        std::string llvmTypeToString(llvm::Type *type);

        void optimize();

        bool generateCode(class Block &root);

        llvm::GenericValue runCode();

        bool preProcessing(class Block &root);

        void setUpBuildIns();

        std::string getType(const std::string& varName);

        void addError() { ++errors; }

        void setMainFunction(llvm::Function *function) { this->mainFunction = function; }

        static bool isBreakingInstruction(llvm::Value *value);

        llvm::Value *
        createMallocCall(llvm::Type *type, int count, const std::string &name, llvm::Value *offset = nullptr);

        llvm::Value *
        createMallocCall(llvm::Type *type, llvm::Value *count, const std::string &name, llvm::Value *offset = nullptr);

        void clearMemory(llvm::Value *ptr, llvm::Value *size);

        llvm::Value *callSizeOf(llvm::Value *arr);

        void createFreeCall(llvm::Value *value);

        bool isKeyFunction(const std::string &name);

        llvm::Value *callKeyFunction(const std::string &name, ExpressionList *args, YYLTYPE location);

        void runtimeError(RuntimeError error);

        llvm::IRBuilder<> *builder;
    private:
        void setCurrentBlock(llvm::BasicBlock *block) { codeBlocks.front()->setCodeBlock(block); }

        bool debug {false};
        std::list<CodeGenBlock *> codeBlocks;
        llvm::Function *initFunction{nullptr};
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
        llvm::Type *doubleType{nullptr};
        llvm::Type *stringType{nullptr};
        llvm::Type *boolType{nullptr};
        llvm::Type *charType{nullptr};
        llvm::Type *voidType{nullptr};
        llvm::Type *varType{nullptr};
        llvm::Type *valType{nullptr};
        llvm::Type *intArrayType{nullptr};
        llvm::Type *doubleArrayType{nullptr};
        llvm::Type *boolArrayType{nullptr};
        std::map<std::string, llvm::Type *> llvmTypeMap;
        std::map<std::string, llvm::Type *> arrayFunctions;
        std::map<std::string, llvm::Type *> castFunctions;
    };
}

#endif /* MLANG_CODEGEN_H */
