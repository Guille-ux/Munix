#ifndef MUNIXCC_BUFFI_H
#define MUNIXCC_BUFFI_H

#include "compat.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MUNIXCC_BUFFER_START_CAP 64
#define MCC_BUFF_GROWTH 2

struct BufferI;

typedef struct BufferI {
	char *buffer;
	size_t size;
	// size_t pos; // de momento no
	size_t cap;
	void (*freeBuff)(struct BufferI *buffer);
	void (*emitText)(struct BufferI *buffer, char *text);
	// de momento no
	// void (*goEnd)(struct BufferI *buffer);
	// void (*goPos)(struct BufferI *buffer, size_t pos);
	// void (*clear)(struct BufferI *buffer);
} BufferI;

void munixcc_initDynamicBuffer(BufferI *buffer);
void MCC_freeDynBuff(BufferI *buffer);
void MCC_emitDynText(BufferI *buffer, char *text);

#endif
