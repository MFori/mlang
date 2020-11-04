/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "buildins.h"
#include <string>
#include <cstdarg>

extern "C" DECLSPEC void print(char *str, ...) {
    va_list argp;
    va_start(argp, str);
    vfprintf(stdout, str, argp);
    va_end(argp);
}

extern "C" DECLSPEC void println(char *str, va_list args) {
    fprintln(stdout, str, args);
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