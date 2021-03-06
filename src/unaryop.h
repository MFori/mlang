/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_UNARYOP_H
#define MLANG_UNARYOP_H

#include "ast.h"

namespace mlang {

    /**
     * Unary operator
     * op rhs
     * or
     * lhs op
     */
    class UnaryOp : public Expression {
    public:
        explicit UnaryOp(int op, Expression *lhs, Expression *rhs, YYLTYPE location)
                : op(op), rhs(rhs), lhs(lhs), location(std::move(location)) {}

        ~UnaryOp() override { delete rhs; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Unary op"; }

    private:
        llvm::Value *incDecCodeGen(CodeGenContext &context);

        int op{0};
        Expression *rhs;
        Expression *lhs;
        YYLTYPE location;
    };

}

#endif /* MLANG_UNARYOP_H */
