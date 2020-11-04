/**
 * Created by Martin on 29.10.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include "ast.h"
#include "codegen.h"

extern int yyparse();

extern int yylex_destroy();

extern FILE *yyin;
extern mlang::Block *programBlock;
extern std::stack<std::string> fileNames;
extern std::vector<std::string> libPaths;
extern int parsing_error;

void usage();

int main(int argc, char **argv) {
    libPaths.push_back("./");

    std::string fileName = argv[1];
    yyin = fopen(fileName.c_str(), "r+");

    if (yyin == nullptr) {
        std::cout << "File " << fileName << "not found. Abort" << std::endl;
        return -1;
    }

    fileNames.push("");       // Add the empty file name after last EOF.
    fileNames.push(fileName);
    if (yyparse() || parsing_error) {
        yylex_destroy();
        return 1;
    }

    if (programBlock == nullptr) {
        std::cout << "Parsing " << fileName << "failed. Abort" << std::endl;
    } else {
        std::ostringstream devNull;
        mlang::CodeGenContext context(false ? devNull : std::cout);
        //context.verbose = verbose;
        //context.debug = debug;
        //if( verbose ) context.printCodeGeneration(*programBlock, std::cout);
        if (context.preProcessing(*programBlock)) {
            if (context.generateCode(*programBlock)) {
                context.runCode();
            }
        }
    }

    if (yyin != nullptr)
        fclose(yyin);
    delete programBlock;
    yylex_destroy();
    return 0;
}