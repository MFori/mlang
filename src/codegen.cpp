/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include <fstream>

#pragma warning(push, 0)

#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm-c/Core.h>
#include <llvm/IR/Verifier.h>

#pragma warning(pop)

#include "codegen.h"

#define MAKE_LLVM_EXTERNAL_NAME(a) #a

namespace mlang {

    CodeGenContext::CodeGenContext(std::ostream &outs, bool debug, bool run) : outs(outs), debug(debug), run(run) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetAsmPrinter();
        module = new llvm::Module("mlang", llvmContext);
    }

    void CodeGenContext::newScope(llvm::BasicBlock *bb, ScopeType sc, llvm::BasicBlock *exitBB) {
        scopeType = sc;
        if (bb == nullptr) {
            bb = llvm::BasicBlock::Create(getGlobalContext(), "scope");
        }
        codeBlocks.push_front(new CodeGenBlock(bb, sc, exitBB));
    }

    void CodeGenContext::endScope() {
        CodeGenBlock *top = codeBlocks.front();
        codeBlocks.pop_front();
        delete top;
        scopeType = codeBlocks.empty() ? ScopeType::GLOBAL_BLOCK : codeBlocks.front()->getScopeType();
    }

    Variable *CodeGenContext::findVariable(const std::string &name, bool onlyLocals) {
        if (scopeType != ScopeType::GLOBAL_BLOCK) {
            auto &names = locals();
            if (names.find(name) != names.end()) {
                return names[name];
            }
            if (onlyLocals) {
                return nullptr;
            }
        }

        for (auto &cb : codeBlocks) {
            auto &names = cb->getValueNames();
            if (names.find(name) != names.end()) {
                return names[name];
            }
        }

        return nullptr;
    }

    bool CodeGenContext::hasVariable(const std::string &name) {
        for (auto &cb : codeBlocks) {
            auto &names = cb->getValueNames();
            if (names.find(name) != names.end()) {
                return true;
            }
        }

        return false;
    }

    llvm::Type *CodeGenContext::typeOf(const class Identifier &type) {
        return typeOf(type.getName());
    }

    llvm::Type *CodeGenContext::typeOf(const std::string &name) {
        if (llvmTypeMap.count(name) != 0) {
            return llvmTypeMap[name];
        }

        return nullptr;
    }

    std::string CodeGenContext::llvmTypeToString(llvm::Type *type) {
        for (auto &it : llvmTypeMap) {
            if (it.second == type) {
                return it.first;
            }
        }

        return nullptr;
    }

    std::string CodeGenContext::getVarType(const std::string &varName) {
        for (auto &cb : codeBlocks) {
            auto &types = cb->getTypeMap();
            if (types.find(varName) != types.end()) {
                return types[varName];
            }
        }
        return std::string("");
    }

    void CodeGenContext::setUpBuildIns() {
        intType = llvm::Type::getInt64Ty(getGlobalContext());
        doubleType = llvm::Type::getDoubleTy(getGlobalContext());
        boolType = llvm::Type::getInt1Ty(getGlobalContext());
        charType = llvm::Type::getInt8Ty(getGlobalContext());
        voidType = llvm::Type::getVoidTy(getGlobalContext());
        stringType = llvm::Type::getInt8PtrTy(getGlobalContext());
        intArrayType = llvm::Type::getInt64PtrTy(getGlobalContext());
        doubleArrayType = llvm::Type::getDoublePtrTy(getGlobalContext());
        boolArrayType = llvm::Type::getInt1PtrTy(getGlobalContext());
        varType = llvm::StructType::create(getGlobalContext(), "var");
        valType = llvm::StructType::create(getGlobalContext(), "val");

        llvmTypeMap["Int"] = intType;
        llvmTypeMap["Double"] = doubleType;
        llvmTypeMap["Bool"] = boolType;
        llvmTypeMap["Char"] = charType;
        llvmTypeMap["Void"] = voidType;
        llvmTypeMap["String"] = stringType;
        llvmTypeMap["IntArray"] = intArrayType;
        llvmTypeMap["DoubleArray"] = doubleArrayType;
        llvmTypeMap["BoolArray"] = boolArrayType;
        llvmTypeMap["var"] = varType;
        llvmTypeMap["val"] = valType;

        arrayFunctions["String"] = stringType;
        arrayFunctions["IntArray"] = intArrayType;
        arrayFunctions["DoubleArray"] = doubleArrayType;
        arrayFunctions["BoolArray"] = boolArrayType;

        castFunctions["toInt"] = intType;
        castFunctions["toDouble"] = doubleType;
        castFunctions["toBool"] = boolType;
        castFunctions["toChar"] = charType;
        castFunctions["toString"] = stringType;

        std::vector<llvm::Type *> argTypesOneInt(1, intType);
        std::vector<llvm::Type *> argTypesInt8Ptr(1, llvm::Type::getInt8PtrTy(getGlobalContext()));
        std::vector<llvm::Type *> argTypesInt64Ptr(1, llvm::Type::getInt64PtrTy(getGlobalContext()));

        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypesInt8Ptr,
                                                         true);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(print),
                                                   getModule());
        llvm::Function::arg_iterator i = f->arg_begin();
        if (i != f->arg_end()) {
            i->setName("format_str");
        }
        buildins.push_back({f, (void *) print});

        ft = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypesInt8Ptr, true);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(println), getModule());
        i = f->arg_begin();
        if (i != f->arg_end()) {
            i->setName("format_str");
        }
        buildins.push_back({f, (void *) println});


        ft = llvm::FunctionType::get(llvm::Type::getInt8Ty(getGlobalContext()), false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(read), getModule());
        buildins.push_back({f, (void *) read});

        ft = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(getGlobalContext()), false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(readLine), getModule());
        buildins.push_back({f, (void *) readLine});

        ft = llvm::FunctionType::get(llvm::Type::getInt64Ty(getGlobalContext()), argTypesInt64Ptr, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(sizeOf), getModule());
        buildins.push_back({f, (void *) sizeOf});

        ft = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypesInt8Ptr, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_error),
                                   getModule());
        buildins.push_back({f, (void *) __mlang_error});

        ft = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypesInt8Ptr, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_rm),
                                   getModule());
        buildins.push_back({f, (void *) __mlang_rm});

        ft = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(getGlobalContext()), argTypesOneInt, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_alloc),
                                   getModule());
        buildins.push_back({f, (void *) __mlang_alloc});

        std::vector<llvm::Type *> castTypes {intType, intType, intType, intType, intType, stringType};
        ft = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(getGlobalContext()), castTypes, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_cast),
                                   getModule());
        buildins.push_back({f, (void *) __mlang_cast});

        std::vector<llvm::Type *> castTypesd {doubleType, intType, intType, stringType};
        ft = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(getGlobalContext()), castTypesd, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_castd),
                                   getModule());
        buildins.push_back({f, (void *) __mlang_castd});

        std::vector<llvm::Type *> scompareTypes {stringType, stringType};
        ft = llvm::FunctionType::get(llvm::Type::getInt64Ty(getGlobalContext()), scompareTypes, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_scompare),getModule());
        buildins.push_back({f, (void *) __mlang_scompare});

        ft = llvm::FunctionType::get(llvm::Type::getInt64Ty(getGlobalContext()), argTypesInt8Ptr, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(len),getModule());
        buildins.push_back({f, (void *) len});

        std::vector<llvm::Type *> copyTypes {stringType, stringType, intType, intType};
        ft = llvm::FunctionType::get(voidType, copyTypes, false);
        f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, MAKE_LLVM_EXTERNAL_NAME(__mlang_copy),getModule());
        buildins.push_back({f, (void *) __mlang_copy});
    }

    void CodeGenContext::optimize() {
        llvm::legacy::FunctionPassManager fpm(getModule());
        llvm::PassManagerBuilder pmbuilder;
        pmbuilder.OptLevel = 3;
        pmbuilder.populateFunctionPassManager(fpm);
        for (auto &fn : getModule()->getFunctionList()) {
            fpm.run(fn);
        }
        fpm.run(*initFunction);
    }

    void CodeGenContext::initMainFunction() {
        llvm::CallInst::Create(initFunction, "", currentBlock());
    }

    bool CodeGenContext::generateCode(Block &root) {
        outs << "Generating code...\n";

        std::vector<llvm::Type *> argTypes;
        llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypes, false);
        initFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "__mlang_init_fun",getModule());
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", initFunction, nullptr);
        setUpBuildIns();

        newScope(bblock, ScopeType::GLOBAL_BLOCK);

        root.codeGen(*this);
        if (errors > 0) {
            outs << "Compilation error(s). Abort.\n";
            return false;
        }
        if (currentBlock()->getTerminator() == nullptr) {
            llvm::ReturnInst::Create(getGlobalContext(), nullptr, currentBlock());
        }
        endScope();

        if(!run && mainFunction == nullptr) {
            ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), argTypes, false);
            mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main",getModule());
            bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", mainFunction, nullptr);
            newScope(bblock, ScopeType::CODE_BLOCK);
            initMainFunction();
            if (currentBlock()->getTerminator() == nullptr) {
                llvm::ReturnInst::Create(getGlobalContext(), nullptr, currentBlock());
            }
            endScope();
        }

        std::string verifyOutputString;
        llvm::raw_string_ostream verifyOutputStream(verifyOutputString);
        if (llvm::verifyModule(*getModule(), &verifyOutputStream)) {
            verifyOutputStream.flush();
            outs << "Module verification errors:\n" << verifyOutputString;
            return false;
        }

        if (!debug) {
            optimize();
        } else {
            outs << LLVMPrintModuleToString((LLVMModuleRef) module);
        }

        return true;
    }

    llvm::GenericValue CodeGenContext::runCode() {
        outs << "Running code...\n";
        std::string err;
        llvm::ExecutionEngine *ee = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).setErrorStr(
                &err).setEngineKind(llvm::EngineKind::JIT).create();
        std::cout << err;
        assert(ee);

        for (auto info : buildins) {
            ee->addGlobalMapping(info.f, info.addr);
        }

        ee->finalizeObject();
        std::vector<llvm::GenericValue> noargs;
        llvm::GenericValue v;
        if (mainFunction != nullptr) {
            v = ee->runFunction(mainFunction, noargs);
        } else {
            v = ee->runFunction(initFunction, noargs);
        }

        delete ee;
        return v;
    }

    void CodeGenContext::saveCode(std::ofstream& out) {
        out << LLVMPrintModuleToString((LLVMModuleRef) module);
    }

    llvm::BasicBlock *CodeGenContext::getExitBlockFromCurrent() {
        for (auto &cb : codeBlocks) {
            auto exitBB = cb->getExitBlock();
            if (exitBB != nullptr) {
                return exitBB;
            }
        }

        return nullptr;
    }

    bool CodeGenContext::isBreakingInstruction(llvm::Value *value) {
        if (value == nullptr) {
            return false;
        }
        return llvm::ReturnInst::classof(value) || llvm::BranchInst::classof(value);
    }

    llvm::Value *
    CodeGenContext::createMallocCall(llvm::Type *type, int count, const std::string &name, llvm::Value *offset) {
        return createMallocCall(type, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), count), name);
    }

    llvm::Value *CodeGenContext::createMallocCall(llvm::Type *type, llvm::Value *count, const std::string &name,
                                                  llvm::Value *offset) {
        auto fun = (module->getOrInsertFunction("__mlang_alloc",
                                                llvm::Type::getInt8PtrTy(llvmContext),
                                                llvm::Type::getInt64Ty(llvmContext)));

        auto typeSize = llvm::ConstantExpr::getSizeOf(type);
        auto totalSize = llvm::BinaryOperator::Create(llvm::Instruction::Mul, typeSize, count, "malloc_size",
                                                      currentBlock());
        if (offset != nullptr) {
            totalSize = llvm::BinaryOperator::Create(llvm::Instruction::Add, totalSize, offset, "malloc_size",
                                                     currentBlock());
        }

        std::vector<llvm::Value *> fargs;
        fargs.push_back(totalSize);
        auto mallocatedSpaceRaw = llvm::CallInst::Create(fun, fargs, "tmp", currentBlock());

        return new llvm::BitCastInst(mallocatedSpaceRaw, llvm::Type::getInt64PtrTy(llvmContext), name, currentBlock());
    }

    void CodeGenContext::createFreeCall(llvm::Value *value) {
        auto fun = (module->getOrInsertFunction("__mlang_rm",
                                                llvm::Type::getVoidTy(llvmContext),
                                                llvm::Type::getInt8PtrTy(llvmContext)));
        std::vector<llvm::Value *> fargs;
        value = llvm::CastInst::CreatePointerCast(value, llvm::Type::getInt8PtrTy(llvmContext), "cast_tmp", currentBlock());
        fargs.push_back(value);
        llvm::CallInst::Create(fun, fargs, "", currentBlock());
    }

    bool CodeGenContext::isKeyFunction(const std::string &name) {
        if (arrayFunctions.count(name) != 0) {
            return true;
        }

        if (castFunctions.count(name) != 0) {
            return true;
        }

        if (name == "sizeOf") {
            return true;
        }

        return false;
    }

    llvm::Value *CodeGenContext::callKeyFunction(const std::string &name, ExpressionList *args, YYLTYPE location) {
        if (arrayFunctions.count(name) != 0) {
            if (args->size() != 1) {
                Node::printError(location, "Invalid number of arguments");
                addError();
                return nullptr;
            }

            auto type = arrayFunctions[name];
            auto arr = new Array(type->getPointerElementType(), args->at(0), location);
            return arr->codeGen(*this);
        }

        if (castFunctions.count(name) != 0) {
            if (args->size() != 1) {
                Node::printError(location, "Invalid number of arguments");
                addError();
                return nullptr;
            }

            auto type = castFunctions[name];
            auto cast = new Cast(type, args->at(0), location);
            return cast->codeGen(*this);
        }

        if (name == "sizeOf") {
            if (args->size() != 1) {
                Node::printError(location, "Invalid number of arguments");
                addError();
                return nullptr;
            }
            return callSizeOf(args->at(0)->codeGen(*this));
        }

        return nullptr;
    }

    void CodeGenContext::runtimeError(RuntimeError error) {
        auto fun = (module->getOrInsertFunction("__mlang_error",
                                                llvm::Type::getInt64Ty(llvmContext),
                                                llvm::Type::getInt64Ty(llvmContext)));

        std::vector<llvm::Value *> fargs;
        fargs.push_back((new Integer((int) error))->codeGen(*this));

        llvm::CallInst::Create(fun, fargs, "tmp", currentBlock());
    }

    void CodeGenContext::clearMemory(llvm::Value *ptr, llvm::Value *size) {
        auto memset = (module->getOrInsertFunction("memset",
                                                   llvm::Type::getInt8PtrTy(llvmContext),
                                                   llvm::Type::getInt64Ty(llvmContext),
                                                   llvm::Type::getInt64Ty(llvmContext)));
        std::vector<llvm::Value *> fargs;
        fargs.push_back(ptr);
        fargs.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmContext), 0));
        fargs.push_back(size);
        llvm::CallInst::Create(memset, fargs, "tmp", currentBlock());
    }

    llvm::Value *CodeGenContext::callSizeOf(llvm::Value *arr) {
        if (arr == nullptr || !arr->getType()->isPointerTy()) {
            Node::printError("sizeOf invalid parameter");
            addError();
            return nullptr;
        }

        auto fun = (module->getOrInsertFunction("sizeOf",
                                                llvm::Type::getInt64Ty(llvmContext),
                                                llvm::Type::getInt64PtrTy(llvmContext)));
        std::vector<llvm::Value *> fargs;
        arr = llvm::CastInst::CreatePointerCast(arr, llvm::Type::getInt64PtrTy(llvmContext), "cast_tmp", currentBlock());
        fargs.push_back(arr);
        llvm::Value *size = llvm::CallInst::Create(fun, fargs, "size_of", currentBlock());
        return new llvm::BitCastInst(size, llvm::Type::getInt64Ty(llvmContext), "size", currentBlock());
    }

    llvm::Type *Variable::getType() {
        if (value == nullptr) {
            return nullptr;
        }

        if (isLocal()) {
            return ((llvm::AllocaInst *) value)->getType()->getElementType();
        } else {
            return ((llvm::GlobalVariable *) value)->getType()->getElementType();
        }
    }
}
