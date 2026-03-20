#ifndef MUNX_H
#define MUNX_H

#include <stdint.h>

#define MUNX_MAGIC 'XNUM' // esta al reves xq estamos en little endian

typedef struct {
	uint32_t magic; // 'MUNX' en binario
	uint32_t min_ram; // RAM mínima
	uint32_t preferred_ram; // ram q al programa le gustaría
	uint32_t entry_point; // punto de entrada al código
} munx_header_t;

void munxInit(void *bin_start, uint32_t min_ram, uint32_t preferred_ram, uint32_t entry_point);

#endif
