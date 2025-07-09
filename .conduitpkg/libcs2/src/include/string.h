#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

void *memset(void* block, unsigned char c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *str);
void byte_to_hex_string(uint8_t byte, char* buffer);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
void *memmove(void *dest, const void *src, size_t n);


#endif
