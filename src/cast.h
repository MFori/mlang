/**
 * Created by Martin on 19.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_CAST_H
#define MLANG_CAST_H

#include "ast.h"

namespace mlang {

    /**
     * Cast functions
     * (toInt(), toString(), ...)
     */
    class Cast : public Expression {
    public:
        explicit Cast(llvm::Type *type, Expression *expr, YYLTYPE location)
                : type(type), expr(expr), location(std::move(location)) {}

        ~Cast() override {
            delete expr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Cast"; }

    private:
        llvm::Type *type;
        Expression *expr;
        YYLTYPE location;
    };

}

#endif /* MLANG_CAST_H */
