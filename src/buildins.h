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

extern "C" DECLSPEC int64_t sizeOf(int64_t *ptr);

extern "C" DECLSPEC int64_t len(char *ptr);

extern "C" DECLSPEC void __mlang_rm(char *ptr);

extern "C" DECLSPEC char *__mlang_alloc(int64_t size);

enum class RuntimeError {
    INVALID_SIZEOF_USAGE,
    INDEX_OUT_OF_RANGE
};


extern "C" DECLSPEC int __mlang_error(int error);

extern "C" DECLSPEC void *__mlang_cast(long long val, int fTy, int fBit, int tTy, int tBit, void *space);

extern "C" DECLSPEC void *__mlang_castd(double val, int fTy, int tTy, void *space);

extern "C" DECLSPEC int64_t __mlang_scompare(const char *s1, const char *s2);

extern "C" DECLSPEC void __mlang_copy(char *dest, const char *source, int64_t size, int64_t offset);

#endif /* MLANG_BUILDINS_H */
