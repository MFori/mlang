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
                : type(type), id(id), assignmentExpr(assignmentExpr), location(location) {}

        VariableDeclaration(Identifier *type, Identifier *id, YYLTYPE location)
                : type(type), id(id), assignmentExpr(nullptr), location(location) {}

        VariableDeclaration(Identifier *id, Expression *assignmentExpr, YYLTYPE location)
                : type(new Identifier("var", location)), id(id), assignmentExpr(assignmentExpr), location(location) {}

        VariableDeclaration(Identifier *id, YYLTYPE location)
                : type(new Identifier("var", location)), id(id), assignmentExpr(nullptr), location(location) {}

        ~VariableDeclaration() override {
            delete assignmentExpr;
            delete id;
            delete type;
        }

        llvm::Value* codeGen(CodeGenContext& context) override;
        NodeType getType() override {return NodeType::VARIABLE;}
        std::string toString() override {return "variable declaration";}
        void accept(Visitor &v) override {v.visitVariableDeclaration(this);}

    protected:
        Identifier *type{nullptr};
        Identifier *id{nullptr};
        Expression *assignmentExpr{nullptr};
        YYLTYPE location;
    };
}

#endif /* MLANG_VARIABLE_H */
