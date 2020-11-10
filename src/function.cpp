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
        std::cout << "Code gen fun decl 1 \n";

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

        std::cout << "Code gen fun decl 2 \n" << type->getName() << "\n";
        llvm::Type *t = context.typeOf(*type);
        std::cout << "Code gen fun decl 2.0.1 \n";
        if (t == nullptr) {
            Node::printError(location, " undefined data type: '" + type->getName() + "'\n");
            context.addError();
            return nullptr;
        }

        std::cout << "Code gen fun decl 2.0.2 " << argTypes.size() << "\n";
        // TODO check return type if it is a structure type !!! May be it should be a ptr to the structure!
        llvm::FunctionType *ftype = llvm::FunctionType::get(t, argTypes, false);
        std::cout << "Code gen fun decl 2.1 \n";
        std::string fname = id->getName();
        std::cout << "Code gen fun decl 2.2 " + fname + "\n";
        llvm::Function *fun = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, fname,
                                                     context.getModule());
        std::cout << "Code gen fun decl 2.3 " + fname + "\n";
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(context.getGlobalContext(), "entry", fun, nullptr);
        std::cout << "Code gen fun decl 2.4 " + fname + "\n";
        context.newScope(bblock, ScopeType::FUNCTION_DECL);

        std::cout << "Code gen fun decl 3 " + fname + "\n";
        llvm::Function::arg_iterator actualArgs = fun->arg_begin();
        for (auto varDecl : *arguments) {
            auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(varDecl->codeGen(context));
            std::string valName = varDecl->getVariableName();
            // TODO a struct is coming as struct alloca, but needed to be a pointer to a struct alloca.
            if (allocaInst) {
                if (allocaInst->getAllocatedType()->isPointerTy()) {
                    valName += "_addr";
                }
                actualArgs->setName(valName);
                new llvm::StoreInst(&(*actualArgs), allocaInst, context.currentBlock());
            }
            ++actualArgs;
        }

        auto blockValue = block->codeGen(context);
        if (blockValue == nullptr) {
            Node::printError(location, id->getName() + "(): Function block returns nothing");
            context.addError();
            return nullptr;
        }
        auto retTy = blockValue->getType();

        if (context.currentBlock()->getTerminator() == nullptr) {
            if (type->getName() == "Void" && retTy->isVoidTy()) {
                llvm::ReturnInst::Create(context.getGlobalContext(), nullptr, context.currentBlock());
            } else {
                llvm::ReturnInst::Create(context.getGlobalContext(), blockValue, context.currentBlock());
            }
        }

        if (fname == "main") {
            context.setMainFunction(fun);
        }

        context.endScope();
        std::cout << "Code gen fun decl 4 " + fname + ", current scope = " + (context.getScopeType() == ScopeType::GLOBAL_BLOCK ? "global" : "not global") + "\n";
        return fun;
    }

    llvm::Value *FunctionCall::codeGen(CodeGenContext &context) {
        std::string functionName = id->getName();

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

        return llvm::CallInst::Create(function, fargs, "", context.currentBlock());
    }

}
