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
#include <utility>

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
    class Range;

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
        CHAR,
        STRING,
        ARRAY,
        IDENTIFIER
    };

    /**
     * AST node
     */
    class Node {
    public:
        virtual ~Node() = default;

        /**
         * Generate code for this node
         */
        virtual llvm::Value *codeGen(CodeGenContext &context) = 0;

        /**
         * Get node type
         */
        virtual NodeType getType() = 0;

        virtual std::string toString() { return "Node"; }

        static void printError(const YYLTYPE& location, const std::string &error) {
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

    /**
     * Expression AST node representation
     */
    class Expression : public Node {
    public:
        ~Expression() override = default;

        std::string toString() override { return "Expression"; }
    };

    /**
     * Statement AST node representation
     */
    class Statement : public Expression {
    public:
        ~Statement() override = default;

        std::string toString() override { return "Statement"; }
    };

    /**
     * Integer AST node representation
     */
    class Integer : public Expression {
    public:
        explicit Integer(long long value) : value(value) {}

        ~Integer() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::INTEGER; }

        std::string toString() override { return "Int"; }

    private:
        long long value{0};
    };

    /**
     * Double AST node representation
     */
    class Double : public Expression {
    public:
        explicit Double(double value) : value(value) {}

        ~Double() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::DOUBLE; }

        std::string toString() override { return "Double"; }

    private:
        double value{0.0};
    };

    /**
     * Boolean AST node representation
     */
    class Boolean : public Expression {
    public:
        explicit Boolean(int const value) : boolVal(value) {}

        ~Boolean() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::BOOLEAN; }

        std::string toString() override { return "Bool"; }

    private:
        int boolVal{0};
    };

    /**
    * Char AST node representation
    */
    class Char : public Expression {
    public:
        explicit Char(char const value) : value(value) {}

        ~Char() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::CHAR; }

        std::string toString() override { return "Char"; }

    private:
        char value{0};
    };

    /**
     * Identifier AST node representation
     */
    class Identifier : public Expression {
    public:
        Identifier(std::string name, YYLTYPE location) : name(std::move(name)), location(std::move(location)) {}

        Identifier(const Identifier &id) : name(id.name), location(id.location) {}

        ~Identifier() override = default;

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::IDENTIFIER; }

        std::string toString() override { return "Identifier"; }

        std::string getName() const { return name; }

    private:
        std::string name;
        YYLTYPE location;
    };

    /**
     * Block AST node representation
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

        std::string toString() override { return "Block"; }
    };

    /**
     * Expression statement AST node representation
     */
    class ExpressionStatement : public Statement {
    public:
        explicit ExpressionStatement(Expression *expression) : expression(expression) {}

        ~ExpressionStatement() override { delete expression; }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Expression statement"; }

    private:
        Expression *expression{nullptr};
    };
}

#endif /* MLANG_AST_H */