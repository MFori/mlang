/**
 * Created by Martin on 29.10.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include "ast.h"
#include "codegen.h"
#include <Windows.h>

extern int yyparse();

extern int yylex_destroy();

extern FILE *yyin;
extern mlang::Block *programBlock;
extern std::stack<std::string> fileNames;
extern int parsing_error;

void help();

std::string getExecutablePath();

int main(int argc, char **argv) {
    bool debug = false;
    bool run = false;
    bool hasSrc = false;
    std::string source;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            help();
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "-r" || arg == "--run") {
            run = true;
        } else {
            if (!hasSrc) {
                source = arg;
                hasSrc = true;
            } else {
                std::cerr << "Only one source file is allowed." << std::endl;
                return 1;
            }
        }
    }

    if (!hasSrc) {
        std::cerr << "No source file provided." << std::endl;
        return 1;
    }

    std::string fileName = source;
    yyin = fopen(fileName.c_str(), "r+");

    if (yyin == nullptr) {
        std::cerr << "File " << fileName << "not found. Abort" << std::endl;
        return 1;
    }

    fileNames.push("");
    fileNames.push(fileName);
    if (yyparse() || parsing_error) {
        yylex_destroy();
        return 1;
    }

    if (programBlock == nullptr) {
        std::cerr << "Parsing " << fileName << "failed. Abort" << std::endl;
    } else {
        std::ostringstream devNull;
        mlang::CodeGenContext context(std::cout, debug, run);

        if (context.generateCode(*programBlock)) {
            if (run) {
                context.runCode();
            } else {
                std::string irFileName = fileName + ".ir";
                std::ofstream out(irFileName);
                context.saveCode(out);
                out.close();
                std::cout << "File with llvm ir (" + fileName + ") generated." << std::endl;

                auto index = fileName.find(".mlang", 0);
                if (index != std::string::npos) {
                    fileName.replace(index, 6, "");
                }

                auto buildinsPath = getExecutablePath() + "\\..\\buildins.bc";

                int res = system(("clang -x ir -o " + fileName + ".exe " + irFileName + " " + buildinsPath +
                                  " -Wno-everything").c_str());
                if (!res) {
                    std::cout << "Executable " + fileName + ".exe generated." << std::endl;
                }
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

std::string getExecutablePath() {
    wchar_t buffer[MAX_PATH];

    HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule != nullptr) {
        GetModuleFileName(hModule, (wchar_t *) buffer, MAX_PATH);
    }

    std::wstring ws(buffer);
    return std::string(ws.begin(), ws.end());
}

void help() {
    std::cout << "Usage:\n";
    std::cout << "mlang filename -h -d -r \n";
    std::cout << "\t-h prints this help text.\n";
    std::cout << "\t-d debug code generation. Disables the code optimizer pass.\n";
    std::cout << "\t-r run program (just in time compilation), otherwise LLVM-IR and executable is generated\n";
}