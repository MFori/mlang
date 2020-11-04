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

        llvm::AllocaInst *var = context.findVariable(lhs->getName());
        if (var == nullptr) {
            /* In this case the type deductions takes place. This is an assignment with the var keyword. */
            llvm::Type *ty = value->getType();
            var = new llvm::AllocaInst(ty, 0, lhs->getName(), context.currentBlock());
            context.locals()[lhs->getName()] = var;
        }

        llvm::Type *varType = var->getType()->getElementType();

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

        return new llvm::StoreInst(value, var, false, context.currentBlock());
    }

}