/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_VISITOR_H
#define MLANG_VISITOR_H

namespace mlang {
    class Expression;
    class Statement;
    class Integer;
    class Double;
    class Boolean;
    class String;
    class Identifier;
    class Block;
    class ExpressionStatement;
    class VariableDeclaration;
    class BinaryOp;
    class UnaryOp;
    class Return;
    class Assignment;
    class FunctionCall;
    class FunctionDeclaration;
    class Conditional;

    class Visitor {
    public:
        virtual void visitExpression(Expression *e) = 0;

        virtual void visitStatement(Statement *e) = 0;

        virtual void visitInteger(Integer *e) = 0;

        virtual void visitDouble(Double *e) = 0;

        virtual void visitBoolean(Boolean *e) = 0;

        virtual void visitString(String *e) = 0;

        virtual void visitIdentifier(Identifier *e) = 0;

        virtual void visitBlock(Block *e) = 0;

        virtual void visitExpressionStatement(ExpressionStatement *e) = 0;

        virtual void visitVariableDeclaration(VariableDeclaration *e) = 0;

        virtual void visitBinaryOp(BinaryOp *e) = 0;

        virtual void visitUnaryOp(UnaryOp *e) = 0;

        virtual void visitReturnStatement(Return *e) = 0;

        virtual void visitAssignment(Assignment *e) = 0;

        virtual void visitFunctionCall(FunctionCall *e) = 0;

        virtual void visitFunctionDeclaration(FunctionDeclaration *e) = 0;

        virtual void visitConditional(Conditional *e) = 0;
    };

}

#endif /* MLANG_VISITOR_H */
