/**
 * Created by Martin on 08.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_STRJOIN_H
#define MLANG_STRJOIN_H

#include "ast.h"

namespace mlang {

    class StringJoin : public Expression {
    public:
        StringJoin(ExpressionList *args, YYLTYPE location) : args(args), location(location) {}

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

#endif /* MLANG_STRJOIN_H */
