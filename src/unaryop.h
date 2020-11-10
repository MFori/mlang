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

    class UnaryOp : public Expression {
    public:
        explicit UnaryOp(int op, Expression *rhs, YYLTYPE location) : op(op), rhs(rhs),  location(location) {}

        ~UnaryOp() override { delete rhs; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "unary op"; }

        void accept(Visitor &v) override { v.visitUnaryOp(this); }

    private:
        int op{0};
        Expression *rhs;
        YYLTYPE location;
    };

}

#endif /* MLANG_UNARYOP_H */
