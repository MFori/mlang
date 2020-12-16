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

    /**
     * While / do-while loop
     * while (cond) { doBlock } - doFirst = false
     * do { doBlock } while (cond) - doFirst = true
     */
    class WhileLoop : public Statement {
    public:
        explicit WhileLoop(Expression *cond, Block *doBlock, bool doFirst, YYLTYPE location)
                : condition(cond), doBlock(doBlock), doFirst(doFirst), location(std::move(location)) {}

        ~WhileLoop() override {
            delete condition;
            delete doBlock;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return doFirst ? "While loop" : "Do-While loop"; }

    private:
        Expression *condition{nullptr};
        Block *doBlock{nullptr};
        bool doFirst{true};
        YYLTYPE location;
    };

}

#endif /* MLANG_WHILELOOP_H */
