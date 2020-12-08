/**
 * Created by Martin on 08.12.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */

#include "str_join.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *StringJoin::codeGen(CodeGenContext &context) {

        std::vector<llvm::Value *> strings;
        llvm::Value *length = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 0);
        llvm::Type *strType = llvm::Type::getInt8PtrTy(context.getGlobalContext());

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

            auto fun = (context.getModule()->getOrInsertFunction("len",
                                                    llvm::Type::getInt64Ty(context.getGlobalContext()),strType));
            std::vector<llvm::Value *> fargs;
            fargs.push_back(arg);
            llvm::Value *len = llvm::CallInst::Create(fun, fargs, "len", context.currentBlock());

            length = llvm::BinaryOperator::Create(llvm::Instruction::Add, length, len, "mathtmp", context.currentBlock());
        }

        return length;
    }

}
