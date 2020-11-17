/**
 * Created by Martin on 04.11.2020.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef MLANG_BUILDINS_H
#define MLANG_BUILDINS_H

#include <cstdio>
#include <cstdint>

#define DECLSPEC __declspec(dllexport)

extern "C" DECLSPEC void print(char *str, ...);

extern "C" DECLSPEC void println(char *str, ...);

extern "C" DECLSPEC void fprint(FILE *stream, char *str, va_list args);

extern "C" DECLSPEC void fprintln(FILE *stream, char *str, va_list args);

extern "C" DECLSPEC int read();

extern "C" DECLSPEC int sizeOf(int64_t *ptr);

extern "C" DECLSPEC void __mlang_rm(char *ptr);

extern "C" DECLSPEC char* __mlang_alloc(int64_t size);

enum class RuntimeError {
    INVALID_SIZEOF_USAGE,
    INDEX_OUT_OF_RANGE
};


extern "C" DECLSPEC int __mlang_error(int error);

#endif /* MLANG_BUILDINS_H */
