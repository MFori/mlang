/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_RETURN_H
#define MLANG_RETURN_H

#include "ast.h"

namespace mlang {

    class Return : public Statement {
    public:
        explicit Return(YYLTYPE location, Expression *expr = nullptr) : returnExpression(expr), location(location) {}

        ~Return() override { delete returnExpression; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "return"; }

        void accept(Visitor &v) override { v.visitReturnStatement(this); }

    private:
        Expression *returnExpression{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_RETURN_H */
