/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
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
        std::cout << "code gen string " << value << "\n";

        // generate the type for the global var
        ArrayType *ArrayTy_0 = ArrayType::get(IntegerType::get(context.getGlobalContext(), 8), value.size() + 1);
        // create global var which holds the constant string.
        auto *gvar_array_str = new GlobalVariable(*context.getModule(), ArrayTy_0, true,
                                                  GlobalValue::PrivateLinkage, nullptr, ".str");
        gvar_array_str->setAlignment(MaybeAlign(1));
        Constant *const_array_str = ConstantDataArray::getString(context.getGlobalContext(), value);
        gvar_array_str->setInitializer(const_array_str);

        // generate access pointer to the string
        std::vector<Constant *> const_ptr_8_indices;
        ConstantInt *const_int = ConstantInt::get(context.getGlobalContext(), APInt(64, StringRef("0"), 10));
        const_ptr_8_indices.push_back(const_int);
        const_ptr_8_indices.push_back(const_int);
        Constant *const_ptr_8 = ConstantExpr::getGetElementPtr(ArrayTy_0, gvar_array_str, const_ptr_8_indices);
        return const_ptr_8;
    }

    llvm::Value *Identifier::codeGen(CodeGenContext &context) {
        std::cout << "code gen id " << name << "\n";

        Variable *var = context.findVariable(name, false);
        if (var != nullptr) {
            return new llvm::LoadInst(var->getType(), var->getValue(), name, false, context.currentBlock());
        }

        Node::printError(location, "undeclared variable " + name);
        context.addError();
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