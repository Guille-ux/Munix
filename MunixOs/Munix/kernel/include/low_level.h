#ifndef LOW_LEVEL
#define LOW_LEVEL

#include <stdint.h>
#include <stddef.h>

typedef union {
	uint8_t raw;
	struct __attribute__((packed)) {
		uint8_t b0 : 1;
		uint8_t b1 : 1;
		uint8_t b2 : 1;
		uint8_t b3 : 1;
		uint8_t b4 : 1;
		uint8_t b5 : 1;
		uint8_t b6 : 1;
		uint8_t b7 : 1;
	} bits;
} __attribute((packed)) byte;

#endif
