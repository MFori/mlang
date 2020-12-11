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

        size_t s = size;
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

    llvm::Value *StringJoin::codeGen(CodeGenContext &context) {
        if(args->size() < 2) {
            Node::printError(location, " invalid join string number of arguments (at least 2)");
            context.addError();
            return nullptr;
        }

        std::vector<llvm::Value *> strings;
        std::vector<llvm::Value *> sizes;
        llvm::Type *strType = llvm::Type::getInt8PtrTy(context.getGlobalContext());
        llvm::Type *intType = llvm::Type::getInt64Ty(context.getGlobalContext());
        llvm::Value *length = llvm::ConstantInt::get(intType, 1);

        for (auto expr : *args) {
            auto arg = expr->codeGen(context);
            if (arg == nullptr) {
                return nullptr;
            }

            if (arg->getType() != strType) {
                Node::printError(location, " invalid join string variable type ");
                context.addError();
                return nullptr;
            }

            strings.push_back(arg);

            auto fun = (context.getModule()->getOrInsertFunction("len", intType, strType));
            std::vector<llvm::Value *> fargs;
            fargs.push_back(arg);
            llvm::Value *len = llvm::CallInst::Create(fun, fargs, "len", context.currentBlock());
            sizes.push_back(len);

            length = llvm::BinaryOperator::Create(llvm::Instruction::Add, length, len, "mathtmp",
                                                  context.currentBlock());
        }

        auto result = (new Array(strType->getPointerElementType(), length, location))->codeGen(context);

        llvm::Type *voidType = llvm::Type::getVoidTy(context.getGlobalContext());
        auto fun = context.getModule()->getOrInsertFunction("__mlang_copy", voidType, strType, strType, intType, intType);
        std::vector<llvm::Value *> fargs;
        llvm::Value* offset = llvm::ConstantInt::get(intType, 0);

        for (int i = 0; i < strings.size(); i++) {
            fargs.clear();
            fargs.push_back(result);
            fargs.push_back(strings[i]);
            fargs.push_back(sizes[i]);
            fargs.push_back(offset);

            llvm::CallInst::Create(fun, fargs, "", context.currentBlock());
            offset = llvm::BinaryOperator::Create(llvm::Instruction::Add, offset, sizes[i], "mathtmp", context.currentBlock());
        }

        return result;
    }

}
