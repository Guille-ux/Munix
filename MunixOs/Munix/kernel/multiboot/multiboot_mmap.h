#ifndef MULTIBOOT_MMAP
#define MULTIBOOT_MMAP

#include <stdint.h>
#include "../memory/bitmap.h"
#include "multiboot_def.h"

#define MB_MMAP_SCANNER_SIGNATURE 0x01F6

typedef struct {
	uint16_t signature;
	size_t mmap_addr;
	size_t mmap_len;
} mb_mmap_info_t;

bool multiboot_mmap_scanner(bitmap_t *bitmap, void *multiboot_info);
void multiboot1_build_mmap_info(mb_mmap_info_t *mmap_info, multiboot_info_t *mbi);

#endif
