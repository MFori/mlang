/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_ASSIGNMENT_H
#define MLANG_ASSIGNMENT_H

#include "ast.h"

namespace mlang {

    /**
     * Assignment (lhs = rhs)
     */
    class Assignment : public Statement {
    public:
        explicit Assignment(Expression *lhs, Expression *rhs, YYLTYPE location)
                : lhs(lhs), rhs(rhs), location(std::move(location)) {}

        ~Assignment() override {
            delete lhs;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "assignment"; }

    private:
        Expression *lhs{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_ASSIGNMENT_H */
