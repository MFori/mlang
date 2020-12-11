/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_BINARYOP_H
#define MLANG_BINARYOP_H

#include "ast.h"

namespace mlang {

    class BinaryOp : public Expression {
    public:
        BinaryOp(Expression *lhs, int op, Expression *rhs, YYLTYPE location)
                : op(op), lhs(lhs), rhs(rhs), location(std::move(location)) {}

        ~BinaryOp() override {
            delete lhs;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "binaryop"; }

        llvm::Value *doubleCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *integerCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *charCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *boolCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *stringCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context);

    private:
        int op{0};
        Expression *lhs{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_BINARYOP_H */
