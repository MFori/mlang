/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "ast.h"
#include "codegen.h"

using namespace std;
using namespace llvm;

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

    llvm::Value *String::codeGen(CodeGenContext &context) {
        // TODO code generation for string
        return nullptr;
    }

    llvm::Value *Identifier::codeGen(CodeGenContext &context) {
        // TODO code generation for identifier
        return nullptr;
    }

    llvm::Value *Block::codeGen(CodeGenContext &context) {
        llvm::Value *last = nullptr;
        for (auto s: statements) {
            last = s->codeGen(context);
        }
        return last;
    }

    llvm::Value *ExpressionStatement::codeGen(CodeGenContext &context) {
        return expression->codeGen(context);
    }
}