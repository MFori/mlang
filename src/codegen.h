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
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/IRBuilder.h>

#pragma warning(pop)

#include "buildins.h"
#include "ast.h"
#include "array.h"
#include "cast.h"

namespace mlang {

    /**
     * Represent current scope
     */
    enum class ScopeType {
        FUNCTION_DECL, // function declaration
        CODE_BLOCK, // code block inside function
        GLOBAL_BLOCK // code block outside functions
    };

    /**
     * Variable scope
     */
    enum class VariableScope {
        GLOBAL,
        LOCAL
    };

    /**
     * Variable representation
     */
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

    /**
     * Generated code block
     */
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

    /**
     * Context
     * used in all nodes in codegen function
     */
    class CodeGenContext {
    public:
        explicit CodeGenContext(std::ostream &outs, bool debug, bool run);

        ~CodeGenContext() { llvm::llvm_shutdown(); }

        /**
         * Return llvm context
         */
        llvm::LLVMContext &getGlobalContext() { return llvmContext; }

        /**
         * Return llvm module
         */
        llvm::Module *getModule() const { return module; }

        /**
         * Create new scope (and block)
         */
        void newScope(llvm::BasicBlock *bb, ScopeType sc, llvm::BasicBlock *exitBB = nullptr);

        /**
         * Close current scope (and block)
         */
        void endScope();

        /**
         * Get current scope type
         */
        ScopeType getScopeType() { return scopeType; }

        /**
         * Get current block
         */
        llvm::BasicBlock *currentBlock() { return codeBlocks.front()->currentBlock(); }

        /**
         * Get next block to jump from this
         * used in loops for break statement
         */
        llvm::BasicBlock *getExitBlockFromCurrent();

        /**
         * set point (block) where put next instructions == new current block
         */
        void setInsertPoint(llvm::BasicBlock *bblock) { setCurrentBlock(bblock); }

        /**
         * Find variable by name
         * @param onlyLocals if true, search only in current block
         */
        Variable *findVariable(const std::string &name, bool onlyLocals = true);

        /**
         * Check if variable with name exists
         */
        bool hasVariable(const std::string &name);

        /**
         * Get local variables
         */
        ValueNames &locals() { return codeBlocks.front()->getValueNames(); }

        /**
         * Get type by its identifier (name)
         */
        llvm::Type *typeOf(const class Identifier &type);

        /**
         * Get type by its name
         */
        llvm::Type *typeOf(const std::string &name);

        /**
         * Set variable type name
         */
        void setVarType(std::string varTypeName, const std::string &varName) {
            codeBlocks.front()->getTypeMap()[varName] = std::move(varTypeName);
        }

        /**
         * Get variable type name
         */
        std::string getVarType(const std::string &varName);

        /**
         * Convert llvm type to type name
         */
        std::string llvmTypeToString(llvm::Type *type);

        /**
         * Run IR code optimalization
         */
        void optimize();

        /**
         * Generate code for block
         */
        bool generateCode(class Block &root);

        /**
         * Run code using JIT compiler
         */
        llvm::GenericValue runCode();

        /**
         * Save IR code to stream
         */
        void saveCode(std::ofstream& out);

        /**
         * Setup build in functions
         */
        void setUpBuildIns();

        void addError() { ++errors; }

        void setMainFunction(llvm::Function *function) { this->mainFunction = function; }

        void initMainFunction();

        /**
         * Check if value is breaking instruction (break/return)
         */
        static bool isBreakingInstruction(llvm::Value *value);

        llvm::Value *
        createMallocCall(llvm::Type *type, int count, const std::string &name, llvm::Value *offset = nullptr);

        llvm::Value *
        createMallocCall(llvm::Type *type, llvm::Value *count, const std::string &name, llvm::Value *offset = nullptr);

        /**
         * Clear memory
         */
        void clearMemory(llvm::Value *ptr, llvm::Value *size);

        /**
         * Call sizeOf build in function
         */
        llvm::Value *callSizeOf(llvm::Value *arr);

        /**
         * Create free call
         */
        void createFreeCall(llvm::Value *value);

        /**
         * Check if fun is 'key' function = internal build in function
         */
        bool isKeyFunction(const std::string &name);

        /**
         * Call 'key' function (internal build in function)
         */
        llvm::Value *callKeyFunction(const std::string &name, ExpressionList *args, YYLTYPE location);

        /**
         * Show runtime error
         */
        void runtimeError(RuntimeError error);

    private:
        /**
         * set current block
         */
        void setCurrentBlock(llvm::BasicBlock *block) { codeBlocks.front()->setCodeBlock(block); }

        bool debug {false};
        bool run {false};
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
