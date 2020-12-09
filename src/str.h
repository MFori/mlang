/**
 * Created by Martin on 16.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_STR_H
#define MLANG_STR_H

#include <utility>

#include "ast.h"

namespace mlang {

    class String : public Expression {
    public:
        explicit String(std::string value) : value(std::move(value)) {}

        ~String() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::STRING; }

        std::string toString() override { return "string"; }

        void accept(Visitor &v) override { v.visitString(this); }

    private:
        std::string value;
    };

    class StringJoin : public Expression {
    public:
        StringJoin(ExpressionList *args, YYLTYPE location) : args(args), location(std::move(location)) {}

        ~StringJoin() override {
            for (auto i : *args) {
                delete i;
            }
            args->clear();
            delete args;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "string join"; }

        void accept(Visitor &v) override { v.visitStringJoin(this); }

    private:
        ExpressionList *args{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_STR_H */
