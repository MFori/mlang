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
        if (!id->getStructName().empty()) {
            std::string className = context.getType(id->getStructName());
            functionName += "%" + className;
        }

        llvm::Function* function = context.getModule()->getFunction(functionName);
        if (function == nullptr) {
            // see if it is a added function to the class like function(classname param,...)
            functionName = id->getName();
            function     = context.getModule()->getFunction(functionName);
            if (function == nullptr) {
                // May be it is a class function, but called like a function w/o class prefix
                // like: class.function() -> function(class parameter)
                functionName = id->getName() + "%" + getTypeNameOfFirstArg(context);
                function     = context.getModule()->getFunction(functionName);

                if (function == nullptr) {
                    // Or it is a function w/ template parameter which will be generated when the parameter types are known.
                    //auto funcdecl = context.getTemplateFunction(id->getName());
                    //if( funcdecl == nullptr ) {
                        Node::printError(location, " no such function '" + id->getName() + "'");
                        context.addError();
                        return nullptr;
                    //}
                }
            }
        }

        std::vector<llvm::Value*> fargs;
        if (!id->getStructName().empty()) {
            // This a class method call, so put the class object onto the stack in order the function has
            // access via a local alloca
            //llvm::Value* val = context.findVariable(id->getStructName());
            //assert(val != nullptr);
            //fargs.push_back(val);
        } else {
            // Check if first parameter is a class object, means variable of a class and a method of this class
            // exists. Then call this method.
            // Since it is possible to call a class.method(arguments)
            // like method(class, arguments).

            /*if (args->size() && args->front()->getType() == NodeType::IDENTIFIER) {
                Identifier* ident = (Identifier*)*(args->begin());
                // Check if it is a var of class type...
                std::string typeName = context.getType(ident->getName());
                llvm::AllocaInst* alloca   = context.findVariable(ident->getName());
                if (alloca != nullptr) {
                    if (alloca->getType()->getElementType()->isStructTy()) {
                        fargs.push_back(alloca);
                        delete ident;
                        args->erase(begin(*args));
                    }
                }
            }*/
        }


        // Put all parameter values onto the stack.
        for (auto expr : *args) {
            std::cout << typeid(expr).name() << "\n";
            auto arg = expr->codeGen(context);
            if (arg == nullptr) {
                return nullptr;
            }
            fargs.push_back(arg);
        }

        if( function == nullptr ) {
            std::cout << "Call function here 4" + functionName;
            // Generate the template function, according to the given parameter types.
           //context.setGenerateTemplatedFunction(true);
           //auto funcdeclTemplate = context.getTemplateFunction(id->getName());
           //auto funcdecl = new FunctionDeclaration(*funcdeclTemplate);
           //auto funcparams = funcdecl->getParameter();
           //for( auto i = 0u; i < funcparams->size(); ++i) {
           //    auto fparam = funcparams->at(i);
           //    // Exchange the var parameter with the type of the real used type by the call.
           //    if( fparam->getIdentifierOfVariablenType().getName() == "var" ) {
           //        auto actualType = new Identifier(context.typeNameOf(fargs[i]->getType()), fparam->getLocation());
           //        auto identifier = new Identifier(fparam->getIdentifierOfVariable());
           //        auto substitudeParam = new VariableDeclaration(actualType, identifier, fparam->getLocation());
           //        funcparams->at(i) = substitudeParam;
           //        delete fparam;
           //    }
           //}
           //// Instantiate the function with the now known parameter types.
           //function = dyn_cast<llvm::Function>(funcdecl->codeGen(context));
           //context.setGenerateTemplatedFunction(false);
           //delete funcdecl;
        }

        return llvm::CallInst::Create(function, fargs, "", context.currentBlock());
    }

    std::string FunctionCall::getTypeNameOfFirstArg(CodeGenContext &context) {
        if (args->size() && args->front()->getType() == NodeType::IDENTIFIER) {
            Identifier* ident = static_cast<Identifier*>(*(args->begin()));
            // Check if it is a var of class type...
            return context.getType(ident->getName());
        }
        return "";
    }


}
