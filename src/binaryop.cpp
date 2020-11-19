/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "binaryop.h"
#include "codegen.h"
#include "parser.hpp"

namespace mlang {

    llvm::Value *BinaryOp::codeGen(CodeGenContext &context) {
        llvm::Value *rhsValue = rhs->codeGen(context);
        llvm::Value *lhsValue = lhs->codeGen(context);
        if (rhsValue == nullptr || lhsValue == nullptr) {
            return nullptr;
        }

        if (rhsValue->getType() != lhsValue->getType()) {
            std::cout << rhsValue->getName().str() + " - " + lhsValue->getName().str() + " \n";
            Node::printError(location, "binary operator incompatible types");
            context.addError();
            return nullptr;
        }

        bool isDoubleTy = rhsValue->getType()->isFloatingPointTy();
        bool isIntTy = rhsValue->getType()->isIntegerTy(64);
        bool isCharTy = rhsValue->getType()->isIntegerTy(8);
        bool isBoolTy = rhsValue->getType()->isIntegerTy(1);
        bool isStringTy = rhsValue->getType() == llvm::Type::getInt8PtrTy(context.getGlobalContext());

        llvm::Value *val = nullptr;
        if (isDoubleTy) {
            val = doubleCodeGen(lhsValue, rhsValue, context);
        } else if (isIntTy) {
            val = integerCodeGen(lhsValue, rhsValue, context);
        } else if (isCharTy) {
            val = charCodeGen(lhsValue, rhsValue, context);
        } else if (isBoolTy) {
            val = boolCodeGen(lhsValue, rhsValue, context);
        } else if (isStringTy) {
            val = stringCodeGen(lhsValue, rhsValue, context);
        }

        if (val == nullptr) {
            Node::printError(location, "unsupported operation");
            context.addError();
        }

        return val;
    }

    llvm::Value *BinaryOp::doubleCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::Instruction::BinaryOps instr;
        switch (op) {
            case TPLUS:
                instr = llvm::Instruction::FAdd;
                break;
            case TMINUS:
                instr = llvm::Instruction::FSub;
                break;
            case TMUL:
                instr = llvm::Instruction::FMul;
                break;
            case TDIV:
                instr = llvm::Instruction::FDiv;
                break;
            default:
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, lhsValue, rhsValue, "mathtmp", context.currentBlock());
    }

    llvm::Value *BinaryOp::integerCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::Instruction::BinaryOps instr;
        switch (op) {
            case TPLUS:
                instr = llvm::Instruction::Add;
                break;
            case TMINUS:
                instr = llvm::Instruction::Sub;
                break;
            case TMUL:
                instr = llvm::Instruction::Mul;
                break;
            case TDIV:
                instr = llvm::Instruction::SDiv;
                break;
            default:
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, lhsValue, rhsValue, "mathtmp", context.currentBlock());
    }

    llvm::Value *BinaryOp::charCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::Instruction::BinaryOps instr;
        switch (op) {
            case TPLUS:
                instr = llvm::Instruction::Add;
                break;
            case TMINUS:
                instr = llvm::Instruction::Sub;
                break;
            case TMUL:
                instr = llvm::Instruction::Mul;
                break;
            case TDIV:
                instr = llvm::Instruction::SDiv;
                break;
            default:
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, lhsValue, rhsValue, "mathtmp", context.currentBlock());
    }

    llvm::Value *BinaryOp::boolCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const {
        llvm::Instruction::BinaryOps instr;
        switch (op) {
            case TAND:
                instr = llvm::Instruction::And;
                break;
            case TOR:
                instr = llvm::Instruction::Or;
                break;
            default:
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, lhsValue, rhsValue, "mathtmp", context.currentBlock());
    }

    llvm::Value *BinaryOp::stringCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) {
        if (op != TPLUS) {
            return nullptr;
        }

        // TODO string concatenation
        return nullptr;
    }

}