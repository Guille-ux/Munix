#include "../include/string.h"


char *strcpy(char *dest, const char *src) {
    char *original_dest = dest;
    while ((*dest++ = *src++) != '\0');
    return original_dest;
}


char *strcat(char *dest, const char *src) {
    char *original_dest = dest;
    while (*dest != '\0') {
        dest++;
    }
    
    while ((*dest++ = *src++) != '\0');
    return original_dest;
}


void byte_to_hex_string(uint8_t byte, char* buffer) {
    const char hex_digits[] = "0123456789ABCDEF";
    buffer[0] = hex_digits[(byte >> 4) & 0x0F];
    buffer[1] = hex_digits[byte & 0x0F];
    buffer[2] = '\0';
}


void *memset(void *block, unsigned char c, size_t n) {
	uint8_t *b=(uint8_t*)block;
	for (size_t i=0;i<n;i++) {
		b[i]=c;
	}
	return block;
}

size_t strlen(const char *str) {
	size_t len=0;
	while (str[len]!='\0') {
		len++;
	}
	return len;
}

void *memcpy(void *dest, const void *src, size_t n) {
	uint8_t *d=(uint8_t*)dest;
	const uint8_t *s=(const uint8_t*)src;

	for (size_t i=0;i<n;i++) {
		d[i]=s[i];
	}

	return dest;
}
