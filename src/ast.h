/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_AST_H
#define MLANG_AST_H

#pragma warning(push, 0)

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#pragma warning(pop)

#include <iostream>

#include "visitor.h"

/**
 * Location type
 * @see https://www.gnu.org/software/bison/manual/html_node/Location-Type.html
 */
typedef struct YYLTYPE {
    int first_line{0};
    int first_column{0};
    int last_line{0};
    int last_column{0};
    std::string file_name;
} YYLTYPE;

namespace mlang {

    class CodeGenContext;

    using StatementList = std::vector<class Statement *>;
    using ExpressionList = std::vector<class Expression *>;
    using VariableList = std::vector<class VariableDeclaration *>;

    enum class NodeType {
        EXPRESSION,
        VARIABLE,
        FUNCTION,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING,
        ARRAY,
        IDENTIFIER
    };

    class Node {
    public:
        virtual ~Node() = default;

        virtual llvm::Value *codeGen(CodeGenContext &context) = 0;

        virtual NodeType getType() = 0;

        virtual void accept(Visitor &v) = 0;

        virtual std::string toString() { return "node"; }

        static void printError(YYLTYPE location, const std::string &error) {
            std::cerr
                    << location.file_name
                    << ": line "
                    << location.first_line << " column "
                    << location.first_column << "-"
                    << location.last_column << ":"
                    << error << std::endl;
        }

        static void printError(const std::string &error) {
            std::cerr << error << std::endl;
        }
    };

    class Expression : public Node {
    public:
        ~Expression() override = default;

        std::string toString() override { return "expression"; }

        void accept(Visitor &v) override { v.visitExpression(this); }
    };

    class Statement : public Expression {

    };

    /**
     * Integer ast node representation
     */
    class Integer : public Expression {
    public:
        explicit Integer(long long value) : value(value) {}

        ~Integer() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::INTEGER; }

        std::string toString() override { return "integer"; }

        void accept(Visitor &v) override { v.visitInteger(this); }

    private:
        long long value{0};
    };

    /**
     * Double ast node representation
     */
    class Double : public Expression {
    public:
        explicit Double(double value) : value(value) {}

        ~Double() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::DOUBLE; }

        std::string toString() override { return "double"; }

        void accept(Visitor &v) override { v.visitDouble(this); }

    private:
        double value{0.0};
    };

    /**
     * Boolean ast node representation
     */
    class Boolean : public Expression {
    public:
        explicit Boolean(int const value) : boolVal(value) {}

        ~Boolean() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::BOOLEAN; }

        std::string toString() override { return "boolean"; }

        void accept(Visitor &v) override { v.visitBoolean(this); }

    private:
        int boolVal{0};
    };

    /**
     * String ast node representation
     */
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

    /**
     * Identifier ast node representation
     */
    class Identifier : public Expression {
    public:
        Identifier(const std::string &name, YYLTYPE location) : name(name), location(location) {}

        Identifier(const Identifier &id) : name(id.name), location(id.location) {}

        ~Identifier() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::IDENTIFIER; }

        std::string toString() override { return "identifier"; }

        void accept(Visitor &v) override { v.visitIdentifier(this); }

        std::string getName() const { return name; }

    private:
        std::string name;
        YYLTYPE location;
    };

    /**
     * Block ast node representation
     */
    class Block : public Expression {
    public:
        StatementList statements;

        Block() = default;

        ~Block() override {
            for (auto s : statements) {
                delete s;
            }
            statements.clear();
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "block"; }

        void accept(Visitor &v) override { v.visitBlock(this); }
    };

    /**
     * Expression statement ast node representation
     */
    class ExpressionStatement : public Statement {
    public:
        explicit ExpressionStatement(Expression *expression) : expression(expression) {}

        ~ExpressionStatement() override { delete expression; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "expression statement"; }

        void accept(Visitor &v) override { v.visitExpressionStatement(this); }

    private:
        Expression *expression{nullptr};
    };
}

#endif /* MLANG_AST_H */