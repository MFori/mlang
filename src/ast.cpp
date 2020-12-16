/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "ast.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Integer::codeGen(CodeGenContext &context) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), value, true);
    }

    llvm::Value *Double::codeGen(CodeGenContext &context) {
        return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.getGlobalContext()), value);
    }

    llvm::Value *Boolean::codeGen(CodeGenContext &context) {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.getGlobalContext()), boolVal);
    }

    llvm::Value *Char::codeGen(CodeGenContext &context) {
        return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context.getGlobalContext()), value);
    }

    llvm::Value *Identifier::codeGen(CodeGenContext &context) {
        Variable *var = context.findVariable(name, false);
        if (var != nullptr && var->getValue() != nullptr) {
            return new llvm::LoadInst(var->getType(), var->getValue(), name, false, context.currentBlock());
        }

        Node::printError(location, "Undeclared variable " + name);
        context.addError();
        return nullptr;
    }

    llvm::Value *Block::codeGen(CodeGenContext &context) {
        llvm::Value *last = nullptr;
        for (auto s: statements) {
            if(s == nullptr) { // empty statement
                continue;
            }
            last = s->codeGen(context);
            if (mlang::CodeGenContext::isBreakingInstruction(last)) {
                break;
            }
        }
        return last;
    }

    llvm::Value *ExpressionStatement::codeGen(CodeGenContext &context) {
        return expression->codeGen(context);
    }

}