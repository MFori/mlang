/**
 * Created by Martin on 15.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_FREE_H
#define MLANG_FREE_H

#include "ast.h"

namespace mlang {

    /**
     * Free memory = rm keyword
     * rm array
     */
    class FreeMemory : public Statement {
    public:
        explicit FreeMemory(Expression *expr, YYLTYPE location) : ptr(expr), location(std::move(location)) {}

        ~FreeMemory() override { delete ptr; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Free"; }

    private:
        Expression *ptr{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FREE_H */
