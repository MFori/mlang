/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_COMPARISON_H
#define MLANG_COMPARISON_H

#include "ast.h"

namespace mlang {

    class Comparison : public Expression {
    public:
        explicit Comparison(Expression *lhs, int op, Expression *rhs, YYLTYPE location) : op(op), lhs(lhs), rhs(rhs),
                                                                                          location(location) {}

        ~Comparison() override {
            delete lhs;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "comparison"; };

        void accept(Visitor &v) override { v.visitComparison(this); }

    private:
        int op{0};
        Expression *lhs{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_COMPARISON_H */
