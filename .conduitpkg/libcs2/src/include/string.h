#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

void *memset(void* block, unsigned char c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *str);

#endif
