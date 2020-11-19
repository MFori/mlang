/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "unaryop.h"
#include "codegen.h"
#include "parser.hpp"
#include "assignment.h"
#include "binaryop.h"

namespace mlang {

    llvm::Value *UnaryOp::codeGen(CodeGenContext &context) {
        if (op == TINC || op == TDEC) {
            return incDecCodeGen(context);
        }

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
                Node::printError(location, "Unknown unary operator");
                context.addError();
                return nullptr;
        }

        return llvm::BinaryOperator::Create(instr, rhsValue, lhsValue, "unarytmp", context.currentBlock());
    }

    llvm::Value *UnaryOp::incDecCodeGen(CodeGenContext &context) {
        int binOperator = op == TINC ? TPLUS : TMINUS;
        llvm::Instruction::BinaryOps instr = op == TINC ? llvm::Instruction::Add : llvm::Instruction::Sub;
        Variable *var = nullptr;
        Expression *expr = lhs != nullptr ? lhs : rhs;
        bool isIdentifier = expr->getType() == NodeType::IDENTIFIER;
        if (isIdentifier) {
            var = context.findVariable(((Identifier *) expr)->getName(), false);
        }

        if (isIdentifier && var != nullptr && var->getValue() != nullptr) {
            bool isDoubleTy = var->getType()->isFloatingPointTy();
            bool isIntTy = var->getType()->isIntegerTy(64);

            if (!isDoubleTy && !isIntTy) {
                Node::printError(location, "unsupported operation");
                context.addError();
                return nullptr;
            }

            Expression *value2;
            if (isIntTy) {
                value2 = new Integer(1);
            } else {
                value2 = new Double(1.0);
            }

            auto assignment = new Assignment((Identifier *) expr,
                                             new BinaryOp((Identifier *) expr, binOperator, value2, location),location);

            if (lhs != nullptr) {
                // i++ i--
                auto allocaInst = new llvm::AllocaInst(var->getType(), 0, "tmp", context.currentBlock());
                auto value = new llvm::LoadInst(var->getType(), var->getValue(), ((Identifier *) lhs)->getName(),
                                                false, context.currentBlock());
                new llvm::StoreInst(value, allocaInst, false, context.currentBlock());
                assignment->codeGen(context);
                return new llvm::LoadInst(var->getType(), allocaInst->getValueName()->getValue(),
                                          allocaInst->getValueName()->getKey(), false, context.currentBlock());
            } else {
                // ++i --i
                return assignment->codeGen(context);
            }
        } else {
            auto value = expr->codeGen(context);

            bool isDoubleTy = value->getType()->isFloatingPointTy();
            bool isIntTy = value->getType()->isIntegerTy();

            if (!isDoubleTy && !isIntTy) {
                Node::printError(location, "unsupported operation");
                context.addError();
                return nullptr;
            }

            llvm::Value *value2;
            if (isIntTy) {
                value2 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 1, true);
            } else {
                value2 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.getGlobalContext()), 1.0);
            }

            auto binaryOperator = llvm::BinaryOperator::Create(instr, value, value2, "tmp", context.currentBlock());

            if (lhs != nullptr) {
                return value;
            } else {
                return binaryOperator;
            }
        }
    }

}