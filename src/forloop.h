/**
 * Created by Martin on 11.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_FORLOOP_H
#define MLANG_FORLOOP_H

#include "ast.h"

namespace mlang {

    class ForLoop : public Statement {
    public:
        explicit ForLoop(YYLTYPE location): location(location) {}

        ~ForLoop() override {

        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "for loop"; }

        void accept(Visitor &v) override { v.visitForLoop(this); }

    private:
        YYLTYPE location;
    };

}

#endif /* MLANG_FORLOOP_H */
