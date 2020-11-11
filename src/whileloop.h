/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_WHILELOOP_H
#define MLANG_WHILELOOP_H

#include "ast.h"

namespace mlang {

    class WhileLoop : public Statement {
    public:
        explicit WhileLoop(Expression *cond, Block *doBlock, bool doFirst, YYLTYPE location)
                : condition(cond), doBlock(doBlock), doFirst(doFirst), location(location) {}

        ~WhileLoop() override {
            delete condition;
            delete doBlock;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "while loop"; }

        void accept(Visitor &v) override { v.visitWhileLoop(this); }

    private:
        Expression *condition{nullptr};
        Block *doBlock{nullptr};
        bool doFirst{true};
        YYLTYPE location;
    };

}

#endif /* MLANG_WHILELOOP_H */
