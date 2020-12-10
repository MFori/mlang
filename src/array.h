/**
 * Created by Martin on 14.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_ARRAY_H
#define MLANG_ARRAY_H

#include <utility>

#include "ast.h"

namespace mlang {

    class Array : public Expression {
    public:
        explicit Array(llvm::Type *type, Expression *size, YYLTYPE location)
                : type(type), size(size), location(std::move(location)) {}

        explicit Array(llvm::Type *type, llvm::Value *count, YYLTYPE location)
                : type(type), count(count), location(std::move(location)) {}

        ~Array() override {
            delete size;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::ARRAY; }

        std::string toString() override { return "array"; }

        void accept(Visitor &v) override { v.visitArray(this); }

        static void validateArrayBounds(llvm::Value *arrayPtr, llvm::Value *index, CodeGenContext &context);

    private:
        llvm::Type *type;
        Expression *size{nullptr};
        llvm::Value *count{nullptr};
        YYLTYPE location;
    };

    class ArrayAccess : public Expression {
    public:
        ArrayAccess(Expression *expr, Expression *index, YYLTYPE location)
                : expr(expr), index(index), location(std::move(location)) {}

        ~ArrayAccess() override {
            delete expr;
            delete index;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::ARRAY; }

        std::string toString() override { return "array access"; }

        void accept(Visitor &v) override { v.visitArrayAccess(this); }

        Expression *getExpression() { return expr; }

        Expression *getIndex() { return index; }

    private:
        Expression *expr{nullptr};
        Expression *index{nullptr};
        YYLTYPE location;
    };
    class ArrayAssignment : public Expression {
    public:
        ArrayAssignment(Expression *lhs, Expression *index, Expression *rhs, YYLTYPE location)
                : lhs(lhs), index(index), rhs(rhs), location(std::move(location)) {}

        ~ArrayAssignment() override {
            delete lhs;
            delete index;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::ARRAY; }

        std::string toString() override { return "array assignment"; }

        void accept(Visitor &v) override { v.visitArrayAssignment(this); }

    private:
        Expression *lhs{nullptr};
        Expression *index{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_ARRAY_H */
