#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void *memset(void *block, int c, size_t n);
int memcmp(void *ab, void *ba, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *str);
void byte_to_hex_string(uint8_t byte, char* buffer);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
void *memmove(void *dest, const void *src, size_t n);
int strcmp(const char *s1, const char *s2);
char *itoa(int value, char *str, int base);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strncat(char *dest, const char *src, size_t n);
char *strtok(char *str, const char *delim);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
size_t strspn(const char *s1, const char *s2);
size_t strcspn(const char *s1, const char *s2);
long atol(const char *str);
char *strdup(const char *str);
char *ltoa(long value, char *buffer, int base);
// char *lltoa(long long value, char *buffer, int base);
char *ultoa(unsigned long value, char *buffer, int base);

// char *llutoa(unsigned long long value, char *buffer, int base);

#endif
