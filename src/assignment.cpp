/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <sstream>
#include "assignment.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Assignment::codeGen(CodeGenContext &context) {
        if (lhs->getType() == NodeType::ARRAY) {
            auto *access = (ArrayAccess *) lhs;
            auto arr = new ArrayAssignment(access->getExpression(), access->getIndex(), rhs, location);
            return arr->codeGen(context);
        } else if(lhs->getType() != NodeType::IDENTIFIER) {
            Node::printError(location, " Invalid assignment");
            context.addError();
            return nullptr;
        }

        auto* ident = (Identifier*) lhs;
        llvm::Value *value = rhs->codeGen(context);

        if (value == nullptr || value->getType()->isVoidTy()) {
            Node::printError(location, " Assignment expression results in nothing");
            context.addError();
            return nullptr;
        }

        if (!context.hasVariable(ident->getName())) {
            Node::printError(location, " Assignment to undefined variable '" + ident->getName() + "'");
            context.addError();
            return nullptr;
        }

        Variable *var = context.findVariable(ident->getName(), false);
        llvm::Type *varType;

        if (var->getValue() == nullptr) {
            /* In this case the type deductions takes place. This is an assignment with the var/val keyword. */
            bool global = context.getScopeType() == ScopeType::GLOBAL_BLOCK;
            llvm::Type *ty = value->getType();

            if (global) {
                llvm::GlobalVariable *gv;
                if (ty->isPointerTy() || !llvm::Constant::classof(value)) {
                    gv = new llvm::GlobalVariable(*context.getModule(), ty, false,
                                                  llvm::GlobalValue::PrivateLinkage,
                                                  llvm::Constant::getNullValue(ty), ident->getName());
                    gv->setAlignment(llvm::MaybeAlign(4));
                    new llvm::StoreInst(value, gv, false, context.currentBlock());
                } else {
                    gv = new llvm::GlobalVariable(*context.getModule(), ty, var->isConst(),
                                                  llvm::GlobalValue::PrivateLinkage,
                                                  (llvm::Constant *) value, ident->getName());
                    gv->setAlignment(llvm::MaybeAlign(4));
                }
                var->setValue(gv);
                return gv;
            } else {
                auto lv = new llvm::AllocaInst(ty, 0, ident->getName(), context.currentBlock());
                var->setValue(lv);
                varType = var->getType();
            }
        } else {
            varType = var->getType();

            std::string typeName = context.getVarType(ident->getName());
            if (typeName == "val") {
                Node::printError(location, "final val '" + ident->getName() + "' cannot be reassignet!\n");
                context.addError();
                return nullptr;
            }
        }

        if (value->getType() != varType) {
            std::stringstream msg;
            msg << " Assignment of incompatible types " << varType->getTypeID() << "(" << varType->getScalarSizeInBits()
                << ") = " << value->getType()->getTypeID()
                << "(" << value->getType()->getScalarSizeInBits() << "). Is a cast missing? ";
            Node::printError(location, msg.str());
            context.addError();
            return nullptr;
        }

        new llvm::StoreInst(value, var->getValue(), false, context.currentBlock());
        return value;
    }

}