/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_FORLOOP_H
#define MLANG_FORLOOP_H

#include "ast.h"
#include "range.h"

namespace mlang {

    /**
     * For loop node
     * for (id in range step step) { block }
     */
    class ForLoop : public Statement {
    public:
        explicit ForLoop(Identifier *id, Range *range, Expression *step, Block *block, YYLTYPE location)
                : ident(id), range(range), step(step), doBlock(block), location(std::move(location)) {}

        ~ForLoop() override {
            delete ident;
            delete range;
            delete step;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "For loop"; }

    private:
        Identifier *ident{nullptr};
        Range *range{nullptr};
        Expression *step{nullptr};
        Block *doBlock{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_FORLOOP_H */
