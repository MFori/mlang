/**
 * Created by Martin on 10.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_COMPARISON_H
#define MLANG_COMPARISON_H

#include "ast.h"

namespace mlang {

    /**
     * Comparison node
     * lhs op rhs
     */
    class Comparison : public Expression {
    public:
        explicit Comparison(Expression *lhs, int op, Expression *rhs, YYLTYPE location)
                : op(op), lhs(lhs), rhs(rhs), location(std::move(location)) {}

        ~Comparison() override {
            delete lhs;
            delete rhs;
        }

        llvm::Value *codeGen(CodeGenContext &context) override;

        NodeType getType() override { return NodeType::EXPRESSION; }

        std::string toString() override { return "Comparison"; };

        llvm::Value *doubleCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *integerCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *charCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *boolCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

        llvm::Value *stringCodeGen(llvm::Value *lhsValue, llvm::Value *rhsValue, CodeGenContext &context) const;

    private:
        int op{0};
        Expression *lhs{nullptr};
        Expression *rhs{nullptr};
        YYLTYPE location;
    };

}

#endif /* MLANG_COMPARISON_H */
