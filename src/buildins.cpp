/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "buildins.h"
#include <string>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <vector>

extern "C" DECLSPEC void print(char *str, ...) {
    va_list argp;
            va_start(argp, str);
    fprint(stdout, str, argp);
            va_end(argp);
}

extern "C" DECLSPEC void println(char *str, ...) {
    va_list argp;
            va_start(argp, str);
    fprintln(stdout, str, argp);
            va_end(argp);
}

extern "C" DECLSPEC void fprint(FILE *const stream, char *str, va_list args) {
    vfprintf(stream, str, args);
}

extern "C" DECLSPEC void fprintln(FILE *const stream, char *str, va_list args) {
    char *out = (char *) malloc(strlen(str) + 2);
    strcpy(out, str);
    strcat(out, "\n");
    vfprintf(stream, out, args);
    free(out);
}

extern "C" DECLSPEC int read() {
    return getchar();
}

extern "C" DECLSPEC int sizeOf(int64_t *ptr) {
    if (ptr == nullptr) {
        __mlang_error((int) RuntimeError::INVALID_SIZEOF_USAGE);
    }
    return ptr[-1];
}

extern "C" DECLSPEC void __mlang_rm(char *ptr) {
    std::cout << "mlang rm called 1 \n";
    free(ptr - 8);
}

extern "C" DECLSPEC char *__mlang_alloc(int64_t size) {
    std::cout << "mlang alloc called 1 \n";
    char *mem = (char *) malloc(size);
    memset(mem, 0, size);
    return mem;
}


std::string errors[] = { // NOLINT(cert-err58-cpp)
        "Invalid sizeof usage!\n",
        "Index out of range!\n"
};

extern "C" DECLSPEC int __mlang_error(int error) {
    std::cout << errors[error];
    exit(1);
}