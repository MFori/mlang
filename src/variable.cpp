/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "variable.h"

#include "codegen.h"

namespace mlang {

    llvm::Value *VariableDeclaration::codeGen(CodeGenContext &context) {
        llvm::Value *val = nullptr;
        // TODO implement code generation
        return val;
    }

}