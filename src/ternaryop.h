/**
 * Created by Martin on 12.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_TERNARYOP_H
#define MLANG_TERNARYOP_H

#include "ast.h"

namespace mlang {

    class TernaryOp : public Expression {
    public:
        TernaryOp(Expression *condExpr, Expression *thenExpr, Expression *elseExpr, YYLTYPE location)
                : condExpr(condExpr), thenExpr(thenExpr), elseExpr(elseExpr), location(location) {}

        ~TernaryOp() override {
            delete condExpr;
            delete thenExpr;
            delete elseExpr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "ternary operator"; }

        void accept(Visitor &v) override { v.visitTernaryOp(this); }

    private:
        Expression *condExpr{nullptr};
        Expression *thenExpr{nullptr};
        Expression *elseExpr{nullptr};
        YYLTYPE location;
    };

}


#endif /* MLANG_TERNARYOP_H */
