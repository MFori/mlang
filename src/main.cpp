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
extern FILE* yyin;
extern mlang::Block* programBlock;
extern std::stack<std::string> fileNames;
extern std::vector<std::string> libPaths;
extern int parsing_error;

void usage();

int main(int argc, char **argv) {


    std::cout << "Start 1";
    std::string fileName = argv[1];
    yyin = fopen(fileName.c_str(), "r+");
    std::cout << "Start 2";

    fileNames.push("");       // Add the empty file name after last EOF.
    fileNames.push(fileName);
    if( yyparse() ) {
        std::cout << "Start 3";
        yylex_destroy();
        std::cout << "Start 4";
        return 1;
    }

    std::cout << "Start 5";
    if( programBlock == nullptr ) {
        std::cout << "Start 6";
        std::cout << "Parsing " << fileName << "failed. Abort" << std::endl;
    } else {
        std::cout << "Start 7";
        std::ostringstream devNull;
        mlang::CodeGenContext context(false ? devNull : std::cout);
        std::cout << "Start 8";
        //context.verbose = verbose;
        //context.debug = debug;
        //if( verbose ) context.printCodeGeneration(*programBlock, std::cout);
        if( context.preProcessing(*programBlock) ) {
            std::cout << "Start 9";
            if( context.generateCode(*programBlock) ) {
                std::cout << "Start 10";
                context.runCode();
                std::cout << "Start 11";
            }
        }
    }
    std::cout << "Start 12";


    if( yyin != nullptr )
        fclose(yyin);
    std::cout << "Start 13";
    delete programBlock;
    std::cout << "Start 14";
    yylex_destroy();
    std::cout << "Start 15";
    return 0;
}