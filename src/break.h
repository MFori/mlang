/**
 * Created by Martin on 12.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_BREAK_H
#define MLANG_BREAK_H

#include "ast.h"

namespace mlang {

    /**
     * Break statement
     * (keyword break)
     */
    class Break : public Statement {
    public:
        explicit Break(YYLTYPE location) : location(std::move(location)) {}

        ~Break() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Break"; }

    private:
        YYLTYPE location;
    };

}

#endif /* MLANG_BREAK_H */
