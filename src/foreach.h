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

    /**
     * Foreach loop node
     * for (id in expr) { block }
     */
    class ForEach : public Statement {
    public:
        explicit ForEach(Identifier *id, Expression *expr, Block *block, YYLTYPE location)
                : ident(id), expr(expr), doBlock(block), location(std::move(location)) {}

        ~ForEach() override {
            delete ident;
            delete expr;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Foreach"; }

    private:
        Identifier *ident{nullptr};
        Expression *expr{nullptr};
        Block *doBlock{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FOREACH_H */
