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
            Node::printError(location, "binary operator incompatible types");
            context.addError();
            return nullptr;
        }

        bool isDoubleTy = rhsValue->getType()->isFloatingPointTy();
        bool isBoolTy = rhsValue->getType()->isIntegerTy(1);

        if ((!isBoolTy && (op == TAND || op == TOR)) || (isBoolTy && (op != TAND && op != TOR))) {
            Node::printError(location, "unsupported operation");
            context.addError();
            return nullptr;
        }

        llvm::Instruction::BinaryOps instr;
        switch (op) {
            case TPLUS:
                instr = isDoubleTy ? llvm::Instruction::FAdd : llvm::Instruction::Add;
                break;
            case TMINUS:
                instr = isDoubleTy ? llvm::Instruction::FSub : llvm::Instruction::Sub;
                break;
            case TMUL:
                instr = isDoubleTy ? llvm::Instruction::FMul : llvm::Instruction::Mul;
                break;
            case TDIV:
                instr = isDoubleTy ? llvm::Instruction::FDiv : llvm::Instruction::SDiv;
                break;
            case TAND:
                instr = llvm::Instruction::And;
                break;
            case TOR:
                instr = llvm::Instruction::Or;
                break;
            default:
                Node::printError(location, "Unknown operator.");
                context.addError();
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, lhsValue, rhsValue, "mathtmp", context.currentBlock());
    }

}