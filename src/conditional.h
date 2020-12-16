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

    /**
     * Conditional = if
     */
    class Conditional : public Statement {
    public:
        /**
         * If with else branch
         */
        explicit Conditional(Expression *condExpr, Expression *thenExpr, Expression *elseExpr, YYLTYPE location)
                : condExpr(condExpr), thenExpr(thenExpr), elseExpr(elseExpr), location(std::move(location)) {}

        /**
         * If without else branch
         */
        explicit Conditional(Expression *condExpr, Expression *thenExpr, YYLTYPE location)
                : condExpr(condExpr), thenExpr(thenExpr), elseExpr(nullptr), location(std::move(location)) {}

        ~Conditional() override {
            delete condExpr;
            delete thenExpr;
            delete elseExpr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Conditional"; }

    private:
        Expression *condExpr{nullptr};
        Expression *thenExpr{nullptr};
        Expression *elseExpr{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_CONDITIONAL_H */
