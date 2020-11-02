/**
 * Created by Martin on 02.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "codegen.h"

namespace mlang {

    CodeGenContext::CodeGenContext(std::ostream &outs) {
        // TODO
    }

    bool CodeGenContext::preProcessing(Block &root) {
        // TODO
        return true;
    }

    bool CodeGenContext::generateCode(Block &root) {
        // TODO
        root.codeGen(*this);
        return true;
    }

    llvm::GenericValue CodeGenContext::runCode() {
        // TODO
        return llvm::GenericValue();
    }


}
