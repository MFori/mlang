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

namespace mlang {

    class FunctionCall : public Statement {
    public:
        explicit FunctionCall(Identifier *id, ExpressionList *args, YYLTYPE location)
                : id(id), args(args), location(location) {}

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

        void accept(Visitor &v) override { v.visitFunctionCall(this); }

        ExpressionList *getArguments() { return args; }

    private:
        std::string getTypeNameOfFirstArg(CodeGenContext& context);

        Identifier *id{nullptr};
        ExpressionList *args{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FUNCTION_H */
