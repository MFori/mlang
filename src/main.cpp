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

void help();

int main(int argc, char **argv) {
    libPaths.emplace_back("./");

    std::string fileName = argv[1];
    yyin = fopen(fileName.c_str(), "r+");
    bool debug = false;

    if (yyin == nullptr) {
        std::cout << "File " << fileName << "not found. Abort" << std::endl;
        return -1;
    }

    fileNames.push("");
    fileNames.push(fileName);
    if (yyparse() || parsing_error) {
        yylex_destroy();
        return 1;
    }

    if (programBlock == nullptr) {
        std::cout << "Parsing " << fileName << "failed. Abort" << std::endl;
    } else {
        std::ostringstream devNull;
        mlang::CodeGenContext context(std::cout, debug);

        if (context.preProcessing(*programBlock)) {
            if (context.generateCode(*programBlock)) {
                context.runCode();
            }
        }
    }

    if (yyin != nullptr) {
        fclose(yyin);
    }
    delete programBlock;
    yylex_destroy();
    return 0;
}

void help() {
    std::cout << "Usage:\n";
    std::cout << "liq filename -h -d -v -q -i path1;path2\n";
    std::cout << "\t-h this help text.\n";
    std::cout << "\t-d debug code generation. Disables the code optimizer pass.\n";
    std::cout << "\t-v be more verbose.\n";
    std::cout << "\t-q be quiet.\n";
    std::cout << "\t-i semicolon separated list of import paths where additional liquid files are located.\n";
}