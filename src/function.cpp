/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "function.h"

namespace mlang {

    llvm::Value *FunctionCall::codeGen(CodeGenContext &context) {
        std::string functionName = id->getName();

        llvm::Function *function = context.getModule()->getFunction(functionName);
        if (function == nullptr) {
            Node::printError(location, " no such function: " + id->getName());
            context.addError();
            return nullptr;
        }

        std::vector<llvm::Value *> fargs;

        for (auto expr : *args) {
            auto arg = expr->codeGen(context);
            if (arg == nullptr) {
                return nullptr;
            }
            fargs.push_back(arg);
        }

        return llvm::CallInst::Create(function, fargs, "", context.currentBlock());
    }

}
