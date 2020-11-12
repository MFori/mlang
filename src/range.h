/**
 * Created by Martin on 12.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_RANGE_H
#define MLANG_RANGE_H

#include "ast.h"

namespace mlang {

    class Range : public Statement {
    public:
        Range(Expression *lhs, int op, Expression *rhs, YYLTYPE location) : op(op), lhs(lhs), rhs(rhs),
                                                                            location(location) {}

        ~Range() override {
            delete lhs;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "range"; }

        void accept(Visitor &v) override { v.visitRange(this); }

        int getOp() const { return op; }

        Expression *getLhs() { return lhs; }

        Expression *getRhs() { return rhs; }

    private:
        int op{0};
        Expression *lhs{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_RANGE_H */
