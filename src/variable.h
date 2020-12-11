/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_VARIABLE_H
#define MLANG_VARIABLE_H

#include "ast.h"

namespace mlang {
    class VariableDeclaration : public Statement {
    public:
        VariableDeclaration(Identifier *type, Identifier *id, Expression *assignmentExpr, YYLTYPE location)
                : type(type), id(id), assignmentExpr(assignmentExpr), location(std::move(location)) {}

        VariableDeclaration(Identifier *type, Identifier *id, YYLTYPE location)
                : type(type), id(id), assignmentExpr(nullptr), location(std::move(location)) {}

        VariableDeclaration(Identifier *id, const std::string &type, Expression *assignmentExpr,
                            const YYLTYPE &location)
                : type(new Identifier(type, location)), id(id), assignmentExpr(assignmentExpr), location(location) {}

        VariableDeclaration(Identifier *id, const std::string &type, const YYLTYPE &location)
                : type(new Identifier(type, location)), id(id), assignmentExpr(nullptr), location(location) {}

        ~VariableDeclaration() override {
            delete assignmentExpr;
            delete id;
            delete type;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::VARIABLE; }

        std::string toString() override { return "variable declaration"; }

        Identifier *getVariableType() const { return type; }

        std::string getVariableName() const { return id->getName(); }

    protected:
        Identifier *type{nullptr};
        Identifier *id{nullptr};
        Expression *assignmentExpr{nullptr};
        YYLTYPE location;
    };
}

#endif /* MLANG_VARIABLE_H */
