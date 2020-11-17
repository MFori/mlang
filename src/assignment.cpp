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
        llvm::Value *value = rhs->codeGen(context);
        
        if (value == nullptr) {
            Node::printError(location, " Assignment expression results in nothing");
            context.addError();
            return nullptr;
        }

        if (!context.hasVariable(lhs->getName())) {
            Node::printError(location, " Assignment to undefined variable '" + lhs->getName() + "'");
            context.addError();
            return nullptr;
        }

        Variable *var = context.findVariable(lhs->getName(), false);
        llvm::Type *varType;

        if (var->getValue() == nullptr) {
            /* In this case the type deductions takes place. This is an assignment with the var/val keyword. */
            bool global = context.getScopeType() == ScopeType::GLOBAL_BLOCK;
            llvm::Type *ty = value->getType();

            if (global) {
                llvm::GlobalVariable* gv;
                if(ty->isPointerTy() || !llvm::Constant::classof(value)) {
                    gv = new llvm::GlobalVariable(*context.getModule(), ty, false,
                                                        llvm::GlobalValue::PrivateLinkage,
                                                  llvm::Constant::getNullValue(ty), lhs->getName());
                    gv->setAlignment(llvm::MaybeAlign(4));
                    new llvm::StoreInst(value, gv, false, context.currentBlock());
                } else {
                    gv = new llvm::GlobalVariable(*context.getModule(), ty, var->isConst(),
                                                        llvm::GlobalValue::PrivateLinkage,
                                                        (llvm::Constant *) value, lhs->getName());
                    gv->setAlignment(llvm::MaybeAlign(4));
                }
                var->setValue(gv);
                return gv;
            } else {
                auto lv = new llvm::AllocaInst(ty, 0, lhs->getName(), context.currentBlock());
                var->setValue(lv);
                varType = var->getType();
            }
        } else {
            varType = var->getType();

            std::string typeName = context.getVarType(lhs->getName());
            if (typeName == "val") {
                Node::printError(location, "final val '" + lhs->getName() + "' cannot be reassignet!\n");
                context.addError();
                return nullptr;
            }
        }

        if (value->getType()->getTypeID() == varType->getTypeID()) {
            // same type but different bit size.
            if (value->getType()->getScalarSizeInBits() > varType->getScalarSizeInBits()) {
                value = llvm::CastInst::CreateTruncOrBitCast(value, varType, "cast", context.currentBlock());
            } else if (value->getType()->getScalarSizeInBits() < varType->getScalarSizeInBits()) {
                value = llvm::CastInst::CreateZExtOrBitCast(value, varType, "cast", context.currentBlock());
            }
        } else if (value->getType() != varType) {
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