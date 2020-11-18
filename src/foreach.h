/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_FOREACH_H
#define MLANG_FOREACH_H

#include "ast.h"

namespace mlang {

    class ForEach : public Statement {
    public:
        explicit ForEach(Identifier *id, Expression *expr, Block *block, YYLTYPE location)
                : ident(id), expr(expr), doBlock(block), location(location) {}

        ~ForEach() override {
            delete ident;
            delete expr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "foreach"; }

        void accept(Visitor &v) override { v.visitForEach(this); }

    private:
        Identifier *ident{nullptr};
        Expression *expr{nullptr};
        Block *doBlock{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FOREACH_H */
