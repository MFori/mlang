/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_CONDITIONAL_H
#define MLANG_CONDITIONAL_H

#include "ast.h"

namespace mlang {

    class Conditional : public Statement {
    public:
        explicit Conditional(Expression *condExpr, Expression *thenExpr, Expression *elseExpr, YYLTYPE location)
                : condExpr(condExpr), thenExpr(thenExpr), elseExpr(elseExpr), location(location) {}

        explicit Conditional(Expression *condExpr, Expression *thenExpr, YYLTYPE location)
                : condExpr(condExpr), thenExpr(thenExpr), elseExpr(nullptr), location(location) {}

        ~Conditional() override {
            delete condExpr;
            delete thenExpr;
            delete elseExpr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "conditional"; }

        void accept(Visitor &v) override { v.visitConditional(this); }

    private:
        Expression *condExpr{nullptr};
        Expression *thenExpr{nullptr};
        Expression *elseExpr{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_CONDITIONAL_H */
