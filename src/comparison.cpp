/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "comparison.h"
#include "codegen.h"
#include "parser.hpp"

namespace mlang {

    llvm::Value* Comparison::codeGen(CodeGenContext &context) {
        llvm::Value* rhsVal = rhs->codeGen(context);
        if (rhsVal == nullptr)
            return nullptr;
        llvm::Value* lhsVal = lhs->codeGen(context);
        if (lhsVal == nullptr)
            return nullptr;
        /*if ((lhsVal->getType() != llvm::Type::getDoubleTy(context.getGlobalContext())) && (lhsVal->getType() != llvm::Type::getInt64Ty(context.getGlobalContext()))) {
            Node::printError("Left hand side of compare expression isn't a value type (number)");
            context.addError();
            return nullptr;
        }
        if ((rhsVal->getType() != llvm::Type::getDoubleTy(context.getGlobalContext())) && (rhsVal->getType() != llvm::Type::getInt64Ty(context.getGlobalContext()))) {
            Node::printError("Right hand side of compare expression isn't a value type (number)");
            context.addError();
            return nullptr;
        }*/
        if (!lhsVal->getType()->isDoubleTy() && !lhsVal->getType()->isIntegerTy()) {
            Node::printError(location,"Left hand side of compare expression isn't a value type (number)");
            context.addError();
            return nullptr;
        }
        if (!rhsVal->getType()->isDoubleTy() && !rhsVal->getType()->isIntegerTy()) {
            Node::printError(location,"Right hand side of compare expression isn't a value type (number)");
            context.addError();
            return nullptr;
        }
        if (rhsVal->getType() != lhsVal->getType()) {
            // since we only support double and int, always cast to double in case of different types.
            auto cinstr = llvm::CastInst::getCastOpcode(rhsVal, true, llvm::Type::getDoubleTy(context.getGlobalContext()), true);
            rhsVal      = llvm::CastInst::Create(cinstr, rhsVal, llvm::Type::getDoubleTy(context.getGlobalContext()), "castdb", context.currentBlock());
            cinstr      = llvm::CastInst::getCastOpcode(lhsVal, true, llvm::Type::getDoubleTy(context.getGlobalContext()), true);
            lhsVal      = llvm::CastInst::Create(cinstr, lhsVal, llvm::Type::getDoubleTy(context.getGlobalContext()), "castdb", context.currentBlock());
        }

        bool                  isDouble = rhsVal->getType() == llvm::Type::getDoubleTy(context.getGlobalContext());
        llvm::Instruction::OtherOps oinstr   = isDouble ? llvm::Instruction::FCmp : llvm::Instruction::ICmp;

        llvm::CmpInst::Predicate predicate;
        switch (op) {
            case TCGE:
                predicate = isDouble ? llvm::CmpInst::FCMP_OGE : llvm::CmpInst::ICMP_SGE;
                break;
            case TCGT:
                predicate = isDouble ? llvm::CmpInst::FCMP_OGT : llvm::CmpInst::ICMP_SGT;
                break;
            case TCLT:
                predicate = isDouble ? llvm::CmpInst::FCMP_OLT : llvm::CmpInst::ICMP_SLT;
                break;
            case TCLE:
                predicate = isDouble ? llvm::CmpInst::FCMP_OLE : llvm::CmpInst::ICMP_SLE;
                break;
            case TCEQ:
                predicate = isDouble ? llvm::CmpInst::FCMP_OEQ : llvm::CmpInst::ICMP_EQ;
                break;
            case TCNE:
                predicate = isDouble ? llvm::CmpInst::FCMP_ONE : llvm::CmpInst::ICMP_NE;
                break;
            default:
                Node::printError(location,"Unknown compare operator.");
                context.addError();
                return nullptr;
        }

        return llvm::CmpInst::Create(oinstr, predicate, lhsVal, rhsVal, "cmptmp", context.currentBlock());
    }

}