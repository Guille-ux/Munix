#include "../include/string.h"
#include "../include/ctype.h"
#include "../include/memory.h"

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

void *memmove(void *dest, const void *src, size_t n) {
	uint8_t *d=(uint8_t*)dest;
	uint8_t *s=(uint8_t*)src;

	if (d == s || n == 0) return dest;

	if (d > s && d < (s + n)) {
		d += n;
		s += n;

		while (n--) {
			*(--d) = *(--s);
		}
	} else {
		while (n--) {
			*(d++) = *(s++);
		}
	}

	return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char *itoa(int value, char *str, int base) {
    int i = 0;
    int isNegative = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (value < 0 && base == 10) {
        isNegative = 1;
        value = -value;
    }

    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10 + 'a') : (rem + '0');
        value = value / base;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate the string

    reverse(str, i); // Reverse the string

    return str;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	if (n==0) return 0;
	do {
		if (*s1 != *s2++) return *(const unsigned char*)s1 - *(const unsigned char*)s2;
        if (*s1++ == '\0') break;
    } while (--n != 0);
    return 0;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *original_dest = dest;
    while (n > 0 && *src != '\0') {
		*dest = *src;
		n--;
	}
	while (n > 0) {
		*dest++ = '\0';
		n--;
	}
    return original_dest;
}
char *strncat(char *dest, const char *src, size_t n) {
    char *original_dest = dest;
    while (*dest != '\0') {
        dest++;
    }
    
    while (n-- > 0 && *src != '\0') {
        *dest++ = *src++;
    }
    *dest='\0';
    return original_dest;
}

static char *strtok_ptr=NULL;

char *strtok(char *str, const char *delim) {
    char *token_start;
    if (str) strtok_ptr=str;
    if (!strtok_ptr || *strtok_ptr=='\0') return NULL;
    
    strtok_ptr += strcspn(strtok_ptr, delim);
    if (*strtok_ptr == '\0') {
        return NULL;
    }
    token_start = strtok_ptr;
    strtok_ptr += strspn(strtok_ptr, delim);
    if (*strtok_ptr!='\0') {
        *strtok_ptr++='\0';
    } else {
        strtok_ptr=NULL;
    }
    return token_start;
}

size_t strcspn(const char *s1, const char *s2) {
    size_t count = 0;
    while (*s1 != '\0' && strchr(s2, *s1) == NULL) { // (Necesitarás strchr)
        count++;
        s1++;
    }
    return count;
}

size_t strspn(const char *s1, const char *s2) {
    size_t count = 0;
    while (*s1 != '\0' && strchr(s2, *s1) != NULL) { // (Necesitarás strchr)
        count++;
        s1++;
    }
    return count;
}

char *strchr(const char *str, int c) {
    while (*str != '\0') {
        if (*str == c) {
            return (char *)str;
        }
        str++;
    }
    if (c == '\0') {
        return (char *)str;
    }
    return NULL;
}

long atol(const char *str) {
	long res=0;
	char sign=1;
	int i=0;

	while (isspace(str[i])) i++; // esto es como un no se, eso .trim() de js
	if (str[i]=='-') {
		sign=-1;
		i++;
	} else if (str[i]=='+') {
		i++;
	}

	while (isdigit(str[i])) {
		res = res * 10 + (str[i]-'0');
		i++;
	}
	return res*sign;
}

char *strdup(const char *str) {
	if (str == NULL) return NULL;

	if (malloc==NULL) return NULL; // hay que inicializar primero las funciones para comunicarse con la memoria, en serio
	size_t len = strlen(str);
	char *new_str=(char*)malloc(len+1);
	if (new_str==NULL) return NULL;
	memcpy(new_str, (const char*)str, len);
	new_str[len]='\0';
	return new_str;
}
