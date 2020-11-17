/**
 * Created by Martin on 16.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "str.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *String::codeGen(CodeGenContext &context) {
        int64_t size = value.size();
        int64_t totalSize = size + 1 + 8;
        llvm::ArrayType *arrTy = llvm::ArrayType::get(llvm::Type::getInt8Ty(context.getGlobalContext()), totalSize);

        int s = size;
        char *sizeParts = new char[8]{0};

        for (int i = 0; i < 8; ++i) {
            sizeParts[i] = s & 0xff;
            s >>= 8;
        }

        const char *val = (char *) value.c_str();
        char *buffer = (char *) malloc(totalSize);
        std::copy(sizeParts, sizeParts + 8, buffer);
        std::copy(val, val + size, buffer + 8);
        buffer[totalSize - 1] = '\0';

        auto i8 = llvm::Type::getInt8Ty(context.getGlobalContext());
        std::vector<llvm::Constant *> chars(totalSize);
        for (unsigned int i = 0; i < totalSize; i++) {
            chars[i] = llvm::ConstantInt::get(i8, buffer[i]);
        }
        auto init = llvm::ConstantArray::get(llvm::ArrayType::get(i8, chars.size()), chars);

        auto gv = new llvm::GlobalVariable(*context.getModule(), init->getType(), true,
                                           llvm::GlobalVariable::ExternalLinkage, init, ".str");

        free(buffer);

        std::vector<llvm::Constant *> indices;
        llvm::ConstantInt *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 0);
        llvm::ConstantInt *start = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 8);

        indices.push_back(zero);
        indices.push_back(start);
        llvm::Constant *const_ptr_8 = llvm::ConstantExpr::getGetElementPtr(arrTy, gv, indices);

        return const_ptr_8;
    }

}