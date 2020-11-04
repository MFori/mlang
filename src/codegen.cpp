/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "llvm/ExecutionEngine/MCJIT.h"
#include <llvm-c/Core.h>

#include "codegen.h"
#include "buildins.h"

#define MAKE_LLVM_EXTERNAL_NAME(a) #a

namespace mlang {

    CodeGenContext::CodeGenContext(std::ostream &outs) : outs(outs) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetAsmPrinter();
        module = new llvm::Module("mlang", llvmContext);
    }

    void CodeGenContext::newScope(llvm::BasicBlock *bb, ScopeType sc) {
        scopeType = sc;
        if (bb == nullptr) {
            bb = llvm::BasicBlock::Create(getGlobalContext(), "scope");
        }
        codeBlocks.push_front(new CodeGenBlock(bb));
    }

    void CodeGenContext::endScope() {
        CodeGenBlock *top = codeBlocks.front();
        codeBlocks.pop_front();
        delete top;
        scopeType = ScopeType::CODE_BLOCK;
    }

    llvm::AllocaInst *CodeGenContext::findVariable(const std::string &name) {
        if (scopeType == ScopeType::FUNCTION_DECL) {
            auto &names = locals();
            if (names.find(name) != names.end()) {
                return names[name];
            }
            return nullptr;
        }

        for (auto &cb : codeBlocks) {
            auto &names = cb->getValueNames();
            if (names.find(name) != names.end()) {
                return names[name];
            }
        }

        return nullptr;
    }

    llvm::Type *CodeGenContext::typeOf(const class Identifier &type) {
        return typeOf(type.getName());
    }

    llvm::Type *CodeGenContext::typeOf(const std::string &name) {
        if (llvmTypeMap.count(name) != 0) {
            return llvmTypeMap[name];
        }

        return voidType;
    }

    bool CodeGenContext::preProcessing(Block &root) {

        // TODO
        return true;
    }

    void CodeGenContext::setUpBuildIns() {
        intType = llvm::Type::getInt64Ty(getGlobalContext());
        doubleType = llvm::Type::getDoubleTy(getGlobalContext());
        stringType = llvm::Type::getInt8PtrTy(getGlobalContext());
        boolType = llvm::Type::getInt1Ty(getGlobalContext());
        voidType = llvm::Type::getVoidTy(getGlobalContext());
        varType = llvm::StructType::create(getGlobalContext(), "var");
        llvmTypeMap["int"] = intType;
        llvmTypeMap["double"] = doubleType;
        llvmTypeMap["string"] = stringType;
        llvmTypeMap["boolean"] = boolType;
        llvmTypeMap["void"] = voidType;
        llvmTypeMap["var"] = varType;

        std::vector<llvm::Type *> argTypesOneInt(1, intType);
        llvm::FunctionType *ft = llvm::FunctionType::get(intType, argTypesOneInt, false);

        std::vector<llvm::Type *> argTypesInt8Ptr(1, llvm::Type::getInt8PtrTy(getGlobalContext()));
        ft = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypesInt8Ptr, true);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(print),
                                                   getModule());
        llvm::Function::arg_iterator i = f->arg_begin();
        if (i != f->arg_end()) {
            i->setName("format_str");
        }
        buildins.push_back({f, (void *) print});
    }

    void CodeGenContext::optimize() {
        llvm::legacy::FunctionPassManager fpm(getModule());
        llvm::PassManagerBuilder builder;
        builder.OptLevel = 3;
        builder.populateFunctionPassManager(fpm);
        for (auto &fn : getModule()->getFunctionList()) {
            fpm.run(fn);
        }
        fpm.run(*mainFunction);
    }

    bool CodeGenContext::generateCode(Block &root) {
        outs << "Generating code...\n";
        // TODO
        std::vector<llvm::Type *> argTypes;
        llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypes, false);
        mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main", getModule());
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
        setUpBuildIns();

        newScope(bblock);
        root.codeGen(*this);
        // TODO

        if (currentBlock()->getTerminator() == nullptr) {
            llvm::ReturnInst::Create(getGlobalContext(), nullptr, currentBlock());
        }

        endScope();

        //if(!debug) {
        //    optimize();
        //}

        return true;
    }

    llvm::GenericValue CodeGenContext::runCode() {
        // TODO
        std::string err;
        llvm::ExecutionEngine *ee = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).setErrorStr(
                &err).setEngineKind(llvm::EngineKind::JIT).create();
        // TODO
        std::cout << err;
        assert(ee);

        ee->finalizeObject();
        std::vector<llvm::GenericValue> noargs;
        llvm::GenericValue v = ee->runFunction(mainFunction, noargs);

        //printf("%s", LLVMPrintModuleToString((LLVMModuleRef)module));

        delete ee;

        return v;
    }

    std::string CodeGenContext::getType(std::string varName) {
        //if (varName == "self") {
        //    return klassName;
        //}
        for (auto &cb : codeBlocks) {
            auto iter = cb->getTypeMap().find(varName);
            if (iter != std::end(cb->getTypeMap())) {
                return cb->getTypeMap()[varName];
            }
        }
        return std::string("");
    }

}
