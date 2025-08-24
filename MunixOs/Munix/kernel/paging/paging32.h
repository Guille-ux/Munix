#ifndef PAGING_32
#define PAGING_32

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../memory/bitmap.h"

typedef struct {
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t us : 1;
	uint32_t pwt : 1;
	uint32_t pcd : 1;
	uint32_t a : 1;
	uint32_t d : 1;
	uint32_t pat : 1;
	uint32_t g : 1;
	uint32_t avl : 3;
	uint32_t frame : 20;
} pt32_entry_t;

typedef struct {
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t us : 1;
	uint32_t pwt : 1;
	uint32_t pcd : 1;
	uint32_t a : 1;
	uint32_t reserved : 1;
	uint32_t ps : 1;
	uint32_t g : 1;
	uint32_t avl : 3;
	uint32_t frame : 20;
} pd32_entry_t;

extern void enable_paging32(uint32_t page_dir) __attribute__((cdecl));

#endif
