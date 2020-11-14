/**
 * Created by Martin on 14.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_ARRAY_H
#define MLANG_ARRAY_H

#include "ast.h"

namespace mlang {

    class Array : public Expression {
    public:
        explicit Array(llvm::Type *type, Expression *size, YYLTYPE location)
                : type(type), size(size), location(location) {}

        ~Array() override {
            delete size;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::ARRAY; }

        std::string toString() override { return "array"; }

        void accept(Visitor &v) override { v.visitArray(this); }

    private:
        llvm::Type *type;
        Expression *size;
        YYLTYPE location;
    };

    class ArrayAccess : public Expression {
    public:
        ArrayAccess(Identifier *id, Expression *index, YYLTYPE location)
                : ident(id), index(index), location(location) {}

        ~ArrayAccess() override {
            delete ident;
            delete index;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "array access"; }

        void accept(Visitor &v) override { v.visitArrayAccess(this); }

    private:
        Identifier *ident{nullptr};
        Expression *index{nullptr};
        YYLTYPE location;
    };

    class ArrayAssignment : public Expression {
    public:
        ArrayAssignment(Identifier *id, Expression *index, Expression *rhs, YYLTYPE location)
                : ident(id), index(index), rhs(rhs), location(location) {}

        ~ArrayAssignment() override {
            delete ident;
            delete index;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "array assignment"; }

        void accept(Visitor &v) override { v.visitArrayAssignment(this); }

    private:
        Identifier *ident{nullptr};
        Expression *index{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_ARRAY_H */
