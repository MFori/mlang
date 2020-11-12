/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_BUILDINS_H
#define MLANG_BUILDINS_H

#include <cstdio>

#define DECLSPEC __declspec(dllexport)

extern "C" DECLSPEC void print(char *str, ...);

extern "C" DECLSPEC void println(char *str, va_list args);

extern "C" DECLSPEC void fprint(FILE *stream, char *str, va_list args);

extern "C" DECLSPEC void fprintln(FILE *stream, char *str, va_list args);

extern "C" DECLSPEC int read();

#endif /* MLANG_BUILDINS_H */
