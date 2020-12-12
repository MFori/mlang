/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "function.h"
#include "variable.h"

namespace mlang {

    llvm::Value *FunctionDeclaration::codeGen(CodeGenContext &context) {
        if (context.getScopeType() != ScopeType::GLOBAL_BLOCK) {
            Node::printError(location, " cannot declare function inside another\n");
            context.addError();
            return nullptr;
        }

        std::vector<llvm::Type *> argTypes;
        for (auto varDecl : *arguments) {
            llvm::Type *ty = context.typeOf(*(varDecl->getVariableType()));
            if (ty == nullptr) {
                Node::printError(location, " undefined data type: '" + varDecl->getVariableType()->getName() + "'\n");
                context.addError();
                return nullptr;
            }
            if (ty->isStructTy()) {
                ty = llvm::PointerType::get(ty, 0);
            }
            argTypes.push_back(ty);
        }

        llvm::Type *t = context.typeOf(*type);
        if (t == nullptr) {
            Node::printError(location, " undefined data type: '" + type->getName() + "'\n");
            context.addError();
            return nullptr;
        }

        llvm::FunctionType *ftype = llvm::FunctionType::get(t, argTypes, false);
        std::string fname = id->getName();

        if (context.isKeyFunction(fname)) {
            Node::printError(location, "Invalid function name");
            context.addError();
            return nullptr;
        }

        llvm::Function *fun = context.getModule()->getFunction(fname);
        if (fun != nullptr) {
            Node::printError(location, " function already exists: " + id->getName());
            context.addError();
            return nullptr;
        }

        bool main = fname == "main";
        auto linkage = main ? llvm::GlobalValue::ExternalLinkage : llvm::GlobalValue::InternalLinkage;

        fun = llvm::Function::Create(ftype, linkage, fname, context.getModule());
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(context.getGlobalContext(), "entry", fun, nullptr);
        context.newScope(bblock, ScopeType::FUNCTION_DECL);

        llvm::Function::arg_iterator actualArgs = fun->arg_begin();
        for (auto varDecl : *arguments) {
            auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(varDecl->codeGen(context));
            std::string valName = varDecl->getVariableName();

            if (allocaInst) {
                if (allocaInst->getAllocatedType()->isPointerTy()) {
                    valName += "_addr";
                }
                actualArgs->setName(valName);
                new llvm::StoreInst(&(*actualArgs), allocaInst, context.currentBlock());
            }
            ++actualArgs;
        }

        if (main) {
            context.initMainFunction();
        }
        auto blockValue = block->codeGen(context);

        if (blockValue == nullptr) {
            if (t->isVoidTy()) {
                llvm::ReturnInst::Create(context.getGlobalContext(), nullptr, context.currentBlock());
            } else {
                Node::printError(location, id->getName() + "(): Function invalid return value");
                context.addError();
                return nullptr;
            }
        } else {
            auto retTy = blockValue->getType();
            bool hasReturn = llvm::ReturnInst::classof(blockValue);
            if (hasReturn) {
                auto returnValue = ((llvm::ReturnInst *) blockValue)->getReturnValue();
                retTy = returnValue == nullptr ? llvm::Type::getVoidTy(context.getGlobalContext())
                                               : returnValue->getType();
            }

            if (!hasReturn && t->isVoidTy()) {
                llvm::ReturnInst::Create(context.getGlobalContext(), nullptr, context.currentBlock());
            } else if (!hasReturn && retTy == t) {
                llvm::ReturnInst::Create(context.getGlobalContext(), blockValue, context.currentBlock());
            } else if (retTy != t) {
                Node::printError(location, id->getName() + "(): Function invalid return value");
                context.addError();
                return nullptr;
            }
        }

        if (main) {
            context.setMainFunction(fun);
        }

        context.endScope();
        return fun;
    }

    llvm::Value *FunctionCall::codeGen(CodeGenContext &context) {
        std::string functionName = id->getName();

        if (context.isKeyFunction(functionName)) {
            llvm::Value *val = context.callKeyFunction(functionName, args, location);
            return val;
        }

        llvm::Function *function = context.getModule()->getFunction(functionName);
        if (function == nullptr) {
            Node::printError(location, " no such function: " + id->getName());
            context.addError();
            return nullptr;
        }

        std::vector<llvm::Value *> fargs;

        for (auto expr : *args) {
            auto arg = expr->codeGen(context);
            if (arg == nullptr) {
                return nullptr;
            }
            fargs.push_back(arg);
        }

        auto fType = function->getFunctionType();
        if ((fType->getNumParams() != fargs.size() && !fType->isVarArg()) ||
            (fargs.size() < fType->getNumParams() && fType->isVarArg())) {
            Node::printError(location, "invalid number of params.");
            context.addError();
            return nullptr;
        }

        for (unsigned int i = 0; i < fType->getNumParams(); i++) {
            if (fType->getParamType(i) != fargs.at(i)->getType()) {
                Node::printError(location, "invalid parameter type");
                context.addError();
                return nullptr;
            }
        }

        return llvm::CallInst::Create(function, fargs, "", context.currentBlock());
    }

}
