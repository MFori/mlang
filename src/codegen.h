/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_CODEGEN_H
#define MLANG_CODEGEN_H

#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Constants.h>

namespace mlang {
    class CodeGenContext {
    public:
        llvm::LLVMContext &getGlobalContext() { return llvmContext; }

    private:
        llvm::LLVMContext llvmContext;
    };
}

#endif /* MLANG_CODEGEN_H */
