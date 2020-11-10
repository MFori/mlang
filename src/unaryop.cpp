/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "unaryop.h"
#include "codegen.h"
#include "parser.hpp"

namespace mlang {

    llvm::Value *UnaryOp::codeGen(CodeGenContext &context) {
        llvm::Value *rhsValue = rhs->codeGen(context);

        bool isDoubleTy = rhsValue->getType()->isFloatingPointTy();
        bool isBoolTy = rhsValue->getType()->isIntegerTy(1);

        if ((!isBoolTy && (op == TNOT)) || (isBoolTy && (op != TNOT))) {
            Node::printError(location, "unsupported operation");
            context.addError();
            return nullptr;
        }

        llvm::Instruction::BinaryOps instr;
        llvm::Value *lhsValue;

        switch (op) {
            case TNOT:
                instr = llvm::Instruction::Xor;
                lhsValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.getGlobalContext()), -1, true);
                break;
            case TINC:
                instr = isDoubleTy ? llvm::Instruction::FAdd : llvm::Instruction::Add;
                if (isDoubleTy) {
                    lhsValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.getGlobalContext()), 1.0);
                } else {
                    lhsValue = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 1, true);
                }
                break;
            case TDEC:
                instr = isDoubleTy ? llvm::Instruction::FAdd : llvm::Instruction::Add;
                if (isDoubleTy) {
                    lhsValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.getGlobalContext()), -1.0);
                } else {
                    lhsValue = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), -1, true);
                }
                break;
            case TPLUS:
                return rhsValue;
            case TMINUS:
                instr = isDoubleTy ? llvm::Instruction::FSub : llvm::Instruction::Sub;
                lhsValue = rhsValue;
                if (isDoubleTy) {
                    lhsValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.getGlobalContext()), 0.0);
                } else {
                    rhsValue = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 0, true);
                }
                break;
            default:
                Node::printError("Unknown unary operator");
                context.addError();
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, rhsValue, lhsValue, "unarytmp", context.currentBlock());
    }

}