/**
 * Created by Martin on 16.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_STR_H
#define MLANG_STR_H

#include "ast.h"

namespace mlang {

    class String : public Expression {
    public:
        explicit String(const std::string &value) : value(value) {}

        ~String() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::STRING; }

        std::string toString() override { return "string"; }

        void accept(Visitor &v) override { v.visitString(this); }

    private:
        std::string value;
    };

}

#endif /* MLANG_STR_H */
