#ifndef PAGING_32
#define PAGING_32

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern void enable_paging32(uint32_t page_dir) __attribute__((cdecl));

#endif
