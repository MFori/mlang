/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_FUNCTION_H
#define MLANG_FUNCTION_H

#include "ast.h"
#include "codegen.h"
#include "variable.h"

namespace mlang {

    class FunctionDeclaration : public Statement {
    public:
        FunctionDeclaration(Identifier *type, Identifier *id, VariableList *args, Block *block, YYLTYPE location)
                : type(type), id(id), arguments(args), block(block), location(std::move(location)) {}

        FunctionDeclaration(Identifier *id, VariableList *args, Block *block, const YYLTYPE &location)
                : type(new Identifier("Void", location)), id(id), arguments(args), block(block), location(location) {}

        ~FunctionDeclaration() override {
            for (auto arg : *arguments) {
                delete arg;
            }
            delete type;
            delete id;
            delete arguments;
            delete block;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::FUNCTION; }

        std::string toString() override { return "function declaration"; }

    private:
        Identifier *type{nullptr};
        Identifier *id{nullptr};
        VariableList *arguments{nullptr};
        Block *block{nullptr};
        YYLTYPE location;
    };

    class FunctionCall : public Statement {
    public:
        explicit FunctionCall(Identifier *id, ExpressionList *args, YYLTYPE location)
                : id(id), args(args), location(std::move(location)) {}

        ~FunctionCall() override {
            for (auto i : *args) {
                delete i;
            }
            args->clear();
            delete args;
            delete id;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "function call"; }

    private:
        Identifier *id{nullptr};
        ExpressionList *args{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FUNCTION_H */
