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
} __attribute__((packed)) pt32_entry_t;

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
} __attribute__((packed)) pd32_entry_t;

typedef union {
	uint32_t raw;
	struct {
		uint32_t offset : 12;
		uint32_t pt_index : 10;
		uint32_t pd_index : 10;
	} __attribute__((packed)) parsed;
} __attribute__((packed)) v_addr32_t;

typedef struct {
	uint8_t rw;
	uint8_t us;
	uint8_t pcd;
	uint8_t pwt;
} __attribute__((packed)) p32_flags_t;

extern void enable_paging32(uint32_t page_dir) __attribute__((cdecl));

void fill_pte32(pt32_entry_t *pte, uint8_t present, uint8_t rw, uint8_t us, uint8_t pwt, uint8_t pcd, uint8_t a, uint8_t d, uint8_t pat, uint8_t g, uint8_t avl, uint32_t frame);

void fill_pde32(pd32_entry_t *pde, uint8_t present, uint8_t rw, uint8_t us, uint8_t pwt, uint8_t pcd, uint8_t a, uint8_t ps, uint8_t g, uint8_t avl, uint32_t frame);

pd32_entry_t *create_page_directory32(bitmap_t *bitmap);
pt32_entry_t *create_page_table32(bitmap_t *bitmap);

void map_page32(bitmap_t *bitmap, pd32_entry_t *page_dir, uint32_t virtual_addr, uint32_t physical_addr, p32_flags_t flags);

void identity_mapping32(bitmap_t *bitmap, pd32_entry_t *page_dir, p32_flags_t flags);

#endif
