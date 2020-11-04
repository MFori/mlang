/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "variable.h"

#include "codegen.h"
#include "assignment.h"

namespace mlang {

    llvm::Value *VariableDeclaration::codeGen(CodeGenContext &context) {
        llvm::Value *val = nullptr;
        if (context.findVariable(id->getName())) {
            Node::printError(location, " variable '" + id->getName() + "' already exist\n");
            context.addError();
            return nullptr;
        }

        llvm::Type *ty = context.typeOf(*type);
        if (ty->isStructTy() && ty->getStructName() == "var") {
            context.locals()[id->getName()] = nullptr;
        } else {
            auto *alloc = new llvm::AllocaInst(ty, 0, id->getName(), context.currentBlock());
            context.locals()[id->getName()] = alloc;
            val = alloc;
        }

        context.setVarType(type->getName(), id->getName());

        if (assignmentExpr != nullptr) {
            Assignment assn(id, assignmentExpr, location);
            assn.codeGen(context);
            id = nullptr;
            assignmentExpr = nullptr;
        }
        return val;
    }
}