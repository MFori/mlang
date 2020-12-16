/**
 * Created by Martin on 19.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "cast.h"
#include "codegen.h"

namespace mlang {

    llvm::Value *Cast::codeGen(CodeGenContext &context) {
        llvm::Value *val = expr->codeGen(context);

        if (val == nullptr) {
            Node::printError(location, "Invalid cast expression");
            context.addError();
            return nullptr;
        }

        llvm::Type *valTy = val->getType();

        if (valTy->isVoidTy() ||
            (valTy->isPointerTy() && valTy != llvm::Type::getInt8PtrTy(context.getGlobalContext()))) {
            Node::printError(location, "Unsupported cast expression");
            context.addError();
            return nullptr;
        }

        if (type == valTy) {
            return val;
        }

        bool srcSigned = !valTy->isIntegerTy(1);
        bool destSigned = !type->isIntegerTy(1);

        if (!type->isPointerTy() && !valTy->isPointerTy()) {
            auto cinstr = llvm::CastInst::getCastOpcode(val, srcSigned, type, destSigned);
            return llvm::CastInst::Create(cinstr, val, type, "cast", context.currentBlock());
        }

        auto intType = llvm::Type::getInt64Ty(context.getGlobalContext());
        auto ptrType = llvm::Type::getInt8PtrTy(context.getGlobalContext());


        llvm::FunctionCallee fun;
        std::vector<llvm::Value *> fargs;

        if (valTy->isDoubleTy()) {
            fun = (context.getModule()->getOrInsertFunction("__mlang_castd", ptrType,
                                                            llvm::Type::getDoubleTy(context.getGlobalContext()),
                                                            intType, intType, ptrType));

            llvm::Value *space = new llvm::AllocaInst(llvm::ArrayType::get(llvm::Type::getInt8Ty(context.getGlobalContext()), 20), 0, "space", context.currentBlock());
            space = llvm::CastInst::CreatePointerCast(space, llvm::Type::getInt8PtrTy(context.getGlobalContext()), "cast_tmp", context.currentBlock());

            fargs.push_back(val);
            fargs.push_back((new Integer((int) valTy->getTypeID()))->codeGen(context));
            fargs.push_back((new Integer((int) type->getTypeID()))->codeGen(context));
            fargs.push_back(space);
        } else {
            fun = (context.getModule()->getOrInsertFunction("__mlang_cast", ptrType, intType, intType, intType,
                                                            intType, intType, ptrType));

            llvm::Value *space;
            if(type->isPointerTy()) {
                space = new llvm::AllocaInst(llvm::ArrayType::get(llvm::Type::getInt8Ty(context.getGlobalContext()), 20), 0, "space", context.currentBlock());
                space = llvm::CastInst::CreatePointerCast(space, llvm::Type::getInt8PtrTy(context.getGlobalContext()), "cast_tmp", context.currentBlock());
            } else {
                space = new llvm::AllocaInst(type, 0, "space", context.currentBlock());
                space = llvm::CastInst::CreatePointerCast(space, llvm::Type::getInt8PtrTy(context.getGlobalContext()), "cast_tmp", context.currentBlock());
            }

            auto cinstr = llvm::CastInst::getCastOpcode(val, srcSigned, llvm::Type::getInt64Ty(context.getGlobalContext()), true);
            auto cast = llvm::CastInst::Create(cinstr, val, llvm::Type::getInt64Ty(context.getGlobalContext()), "cast", context.currentBlock());

            fargs.push_back(cast);
            fargs.push_back((new Integer((int) valTy->getTypeID()))->codeGen(context));
            fargs.push_back((new Integer(valTy->isIntegerTy() ? valTy->getIntegerBitWidth() : 0))->codeGen(context));
            fargs.push_back((new Integer((int) type->getTypeID()))->codeGen(context));
            fargs.push_back((new Integer(type->isIntegerTy() ? type->getIntegerBitWidth() : 0))->codeGen(context));
            fargs.push_back(space);
        }

        llvm::Value *castPtr = llvm::CallInst::Create(fun, fargs, "cast", context.currentBlock());

        if (!valTy->isPointerTy()) {
            auto ispace = new llvm::BitCastInst(castPtr, llvm::Type::getInt64PtrTy(context.getGlobalContext()), "tmp", context.currentBlock());

            llvm::Value *indices[1] = {llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getGlobalContext()), 1)};
            llvm::Value *elementPtr = llvm::GetElementPtrInst::Create(nullptr, ispace, indices, "elem_ptr",
                                                                      context.currentBlock());

            return new llvm::BitCastInst(elementPtr, type, "str", context.currentBlock());
        } else {
            // fromString
            castPtr = llvm::CastInst::CreatePointerCast(castPtr, type->getPointerTo(), "cast", context.currentBlock());
            return new llvm::LoadInst(type, castPtr, "item", context.currentBlock());
        }
    }

}