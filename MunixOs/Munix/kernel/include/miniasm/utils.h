#ifndef MINIASM_UTILS_H
#define MINIASM_UTILS_H

#include <stdint.h>
#include <stddef.h>

size_t getNum(const char *string, size_t *pos);
size_t atob(const char *string, size_t *pos); // la usare para convertir binario
size_t matol(const char *string, size_t *pos);
size_t atoh(const char *string, size_t *pos);

#endif
