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
        if (context.findVariable(id->getName())) {
            Node::printError(location, " variable '" + id->getName() + "' already exist\n");
            context.addError();
            return nullptr;
        }

        llvm::Type *ty = context.typeOf(*type);
        if (ty == nullptr) {
            Node::printError(location, " undefined data type '" + type->getName() + "'\n");
            context.addError();
            return nullptr;
        }

        Variable *variable;
        bool global = context.getScopeType() == ScopeType::CODE_BLOCK;

        if (ty->isStructTy()) {
            bool constant = ty->getStructName() == "val";
            if (global) {
                variable = Variable::newGlobal(nullptr, constant);
            } else {
                variable = Variable::newLocal(nullptr, constant);
            }
        } else {
            if (global) {
                auto *gv = new llvm::GlobalVariable(*context.getModule(), ty, true, llvm::GlobalValue::PrivateLinkage,
                                                    llvm::Constant::getNullValue(ty), id->getName());
                variable = Variable::newGlobal(gv);
            } else {
                auto *alloc = new llvm::AllocaInst(ty, 0, id->getName(), context.currentBlock());
                variable = Variable::newLocal(alloc);
            }
        }

        context.locals()[id->getName()] = variable;
        context.setVarType(type->getName(), id->getName());

        if (assignmentExpr != nullptr) {
            Assignment assn(id, assignmentExpr, location);
            assn.codeGen(context);
            id = nullptr;
            assignmentExpr = nullptr;
        }
        return variable->getValue();
    }
}