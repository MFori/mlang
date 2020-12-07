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

    llvm::Value *Comparison::codeGen(CodeGenContext &context) {
        llvm::Value *rhsVal = rhs->codeGen(context);
        llvm::Value *lhsVal = lhs->codeGen(context);

        if (lhsVal == nullptr || rhsVal == nullptr) {
            Node::printError(location, "unsupported operation");
            context.addError();
            return nullptr;
        }

        if (rhsVal->getType() != lhsVal->getType()) {
            Node::printError(location, "Comparison of incompatible types");
            context.addError();
            return nullptr;
        }

        bool isDoubleTy = rhsVal->getType()->isFloatingPointTy();
        bool isIntTy = rhsVal->getType()->isIntegerTy(64);
        bool isCharTy = rhsVal->getType()->isIntegerTy(8);
        bool isBoolTy = rhsVal->getType()->isIntegerTy(1);
        bool isStringTy = rhsVal->getType() == llvm::Type::getInt8PtrTy(context.getGlobalContext());

        llvm::Value *val = nullptr;
        if (isDoubleTy) {
            val = doubleCodeGen(lhsVal, rhsVal, context);
        } else if (isIntTy) {
            val = integerCodeGen(lhsVal, rhsVal, context);
        } else if (isCharTy) {
            val = charCodeGen(lhsVal, rhsVal, context);
        } else if (isBoolTy) {
            val = boolCodeGen(lhsVal, rhsVal, context);
        } else if (isStringTy) {
            val = stringCodeGen(lhsVal, rhsVal, context);
        }

        if (val == nullptr) {
            Node::printError(location, "unsupported operation");
            context.addError();
        }

        return val;
    }

    llvm::Value *
    Comparison::doubleCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::CmpInst::Predicate predicate;
        switch (op) {
            case TCGE:
                predicate = llvm::CmpInst::FCMP_OGE;
                break;
            case TCGT:
                predicate = llvm::CmpInst::FCMP_OGT;
                break;
            case TCLT:
                predicate = llvm::CmpInst::FCMP_OLT;
                break;
            case TCLE:
                predicate = llvm::CmpInst::FCMP_OLE;
                break;
            case TCEQ:
                predicate = llvm::CmpInst::FCMP_OEQ;
                break;
            case TCNE:
                predicate = llvm::CmpInst::FCMP_ONE;
                break;
            default:
                return nullptr;
        }

        return llvm::CmpInst::Create(llvm::Instruction::FCmp, predicate, lhsValue, rhsValue, "cmptmp",
                                     context.currentBlock());
    }

    llvm::Value *
    Comparison::integerCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::CmpInst::Predicate predicate;
        switch (op) {
            case TCGE:
                predicate = llvm::CmpInst::ICMP_SGE;
                break;
            case TCGT:
                predicate = llvm::CmpInst::ICMP_SGT;
                break;
            case TCLT:
                predicate = llvm::CmpInst::ICMP_SLT;
                break;
            case TCLE:
                predicate = llvm::CmpInst::ICMP_SLE;
                break;
            case TCEQ:
                predicate = llvm::CmpInst::ICMP_EQ;
                break;
            case TCNE:
                predicate = llvm::CmpInst::ICMP_NE;
                break;
            default:
                return nullptr;
        }

        return llvm::CmpInst::Create(llvm::Instruction::ICmp, predicate, lhsValue, rhsValue, "cmptmp",
                                     context.currentBlock());
    }

    llvm::Value *Comparison::charCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        return integerCodeGen(lhsValue, rhsValue, context);
    }

    llvm::Value *Comparison::boolCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::CmpInst::Predicate predicate;
        switch (op) {
            case TCEQ:
                predicate = llvm::CmpInst::ICMP_EQ;
                break;
            case TCNE:
                predicate = llvm::CmpInst::ICMP_NE;
                break;
            default:
                return nullptr;
        }

        return llvm::CmpInst::Create(llvm::Instruction::ICmp, predicate, lhsValue, rhsValue, "cmptmp",
                                     context.currentBlock());
    }

    llvm::Value *Comparison::stringCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) {
        llvm::CmpInst::Predicate predicate;
        switch (op) {
            case TCGE:
                predicate = llvm::CmpInst::ICMP_SGE;
                break;
            case TCGT:
                predicate = llvm::CmpInst::ICMP_SGT;
                break;
            case TCLT:
                predicate = llvm::CmpInst::ICMP_SLT;
                break;
            case TCLE:
                predicate = llvm::CmpInst::ICMP_SLE;
                break;
            case TCEQ:
                predicate = llvm::CmpInst::ICMP_EQ;
                break;
            case TCNE:
                predicate = llvm::CmpInst::ICMP_NE;
                break;
            default:
                return nullptr;
        }
        // TODO string comparison
        return nullptr;
    }
}