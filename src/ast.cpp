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
        // TODO code generation for string

        // generate the type for the global var
        ArrayType* ArrayTy_0 = ArrayType::get(IntegerType::get(context.getGlobalContext(), 8), value.size() +1 );
        // create global var which holds the constant string.
        GlobalVariable* gvar_array__str = new GlobalVariable(*context.getModule(),
                /*Type=*/ArrayTy_0,
                /*isConstant=*/true,
                                                             GlobalValue::PrivateLinkage,
                /*Initializer=*/0, // has initializer, specified below
                                                             ".str");
        gvar_array__str->setAlignment(MaybeAlign(1));
        // create the contents for the string global.
        Constant* const_array_str =  ConstantDataArray::getString(context.getGlobalContext(), value);
        // Initialize the global with the string
        gvar_array__str->setInitializer(const_array_str);

        // generate access pointer to the string
        std::vector<Constant*> const_ptr_8_indices;
        ConstantInt* const_int = ConstantInt::get(context.getGlobalContext(), APInt(64, StringRef("0"), 10));
        const_ptr_8_indices.push_back(const_int);
        const_ptr_8_indices.push_back(const_int);
        Constant* const_ptr_8 = ConstantExpr::getGetElementPtr(ArrayTy_0, gvar_array__str, const_ptr_8_indices);
        return const_ptr_8;
    }

    llvm::Value *Identifier::codeGen(CodeGenContext &context) {
        std::cout << "code gen id " << name << "\n";
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