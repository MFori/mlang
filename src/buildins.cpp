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

extern "C" DECLSPEC int64_t sizeOf(int64_t *ptr) {
    if (ptr == nullptr) {
        __mlang_error((int) RuntimeError::INVALID_SIZEOF_USAGE);
    }
    return ptr[-1];
}

extern "C" DECLSPEC int64_t len(char *ptr) {
    int64_t size = sizeOf((int64_t*) ptr);
    return strnlen(ptr, size);
}

extern "C" DECLSPEC void __mlang_rm(char *ptr) {
    free(ptr - 8);
}

extern "C" DECLSPEC char *__mlang_alloc(int64_t size) {
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

extern "C" DECLSPEC void *__mlang_cast(long long val, int fTy, int fBit, int tTy, int tBit, void *space) {
    if (tTy == 14) {
        // toString
        if (fTy == 12) {
            // int type
            char *c = nullptr;
            size_t size = 0;
            switch (fBit) {
                case 1: {
                    auto str = std::string(((bool) val) ? "true" : "false");
                    size = str.size();
                    c = new char[size];
                    strcpy(c, str.c_str());
                    break;
                }
                case 8: {
                    size = 1;
                    c = new char[size];
                    c[0] = (char) val;
                    break;
                }
                case 64: {
                    auto str = std::to_string(val);
                    size = str.size();
                    c = new char[size];
                    strcpy(c, str.c_str());
                    break;
                }
            }

            if (c == nullptr) {
                return nullptr;
            }

            int s = size;
            char *sizeParts = new char[8]{0};

            for (int i = 0; i < 8; ++i) {
                sizeParts[i] = s & 0xff;
                s >>= 8;
            }

            char *buffer = (char *) space;
            std::copy(sizeParts, sizeParts + 8, buffer);
            std::copy(c, c + size, buffer + 8);
            buffer[size + 8] = '\0';
            delete[] c;

            return buffer;
        }
    } else {
        // from String

        if (tTy == 12) {
            // in type
            switch (tBit) {
                case 1: {
                    char *str = (char *) val;
                    int *i = (int *) space;
                    if (strcmp(str, "true") != 0) {
                        i[0] = 0;
                    } else {
                        i[0] = 1;
                    }
                    return i;
                }
                case 8: {
                    return (char *) val;
                }
                case 64: {
                    int *i = (int *) space;
                    int ii = std::stoi((char *) val);
                    i[0] = ii;
                    return i;
                }
                default:
                    return nullptr;
            }
        } else if (tTy == 3) {
            // double ty
            auto *d = (double *) space;
            double dd = std::stod((char *) val);
            d[0] = dd;
            return d;
        }

    }

    return nullptr;
}

extern "C" DECLSPEC void *__mlang_castd(double val, int fTy, int tTy, void *space) {
    if (tTy == 14) {
        // toString
        if (fTy == 3) {
            // double ty
            auto str = std::to_string((double) val);
            size_t size = str.size();
            char *c = new char[size];
            strcpy(c, str.c_str());

            size_t s = size;
            char *sizeParts = new char[8]{0};

            for (int i = 0; i < 8; ++i) {
                sizeParts[i] = s & 0xff;
                s >>= 8;
            }

            char *buffer = (char *) space;
            std::copy(sizeParts, sizeParts + 8, buffer);
            std::copy(c, c + size, buffer + 8);
            buffer[size + 8] = '\0';
            delete[] c;


            return buffer;
        }
    }

    return nullptr;
}

extern "C" DECLSPEC int64_t __mlang_scompare(const char *s1, const char *s2) {
    size_t len1 =  ((int64_t*) s1)[-1];
    size_t len2 =  ((int64_t*) s2)[-1];

    int cmp = std::strncmp(s1, s2, std::min(len1, len2));

    if(cmp == 0 && len1 != len2) {
        if(len1 > len2) {
            return 1;
        } else {
            return -1;
        }
    }

    return cmp;
}

extern "C" DECLSPEC void __mlang_copy(char *dest, const char *source, int64_t size, int64_t offset) {
    std::memcpy(dest + offset, source, size);
}