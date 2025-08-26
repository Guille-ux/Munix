#include "paging32.h"
#include "../include/libcs2.h"
#include "../memory/bitmap.h"

void fill_pte32(pt32_entry_t *pte, uint8_t present, uint8_t rw, uint8_t us, uint8_t pwt, uint8_t pcd, uint8_t a, uint8_t d, uint8_t pat, uint8_t g, uint8_t avl, uint32_t frame) {
	pte->present = present;
	pte->rw = rw;
	pte->us = us;
	pte->pwt = pwt;
	pte->pcd = pcd;
	pte->a = a;
	pte->d = d;
	pte->pat = pat;
	pte->g = g;
	pte->avl = avl;
	pte->frame = frame;
}

void fill_pde32(pd32_entry_t *pde, uint8_t present, uint8_t rw, uint8_t us, uint8_t pwt, uint8_t pcd, uint8_t a, uint8_t ps, uint8_t g, uint8_t avl, uint32_t frame) {
	pde->present = present;
	pde->rw = rw;
	pde->us = us;
	pde->pwt = pwt;
	pde->pcd = pcd;
	pde->a = a;
	pde->ps = ps;
	pde->g = g;
	pde->avl = avl;
	pde->frame = frame;
}

pd32_entry_t *create_page_directory32(bitmap_t *bitmap) {
	void *page_dir_ptr = bitmap_alloc(bitmap);
	
	if (page_dir_ptr == ((void*)((uint32_t)-1))) return NULL;
	
	memset(page_dir_ptr, 0, 4096);
	
	pd32_entry_t *page_directory = (pd32_entry_t*)page_dir_ptr;

	return page_directory;
}

pt32_entry_t *create_page_table32(bitmap_t *bitmap) {
	void *page_table_ptr = bitmap_alloc(bitmap);
	
	if (page_table_ptr == ((void*)((uint32_t)-1))) return NULL;
	
	memset(page_table_ptr, 0, 4096);
	
	pt32_entry_t *page_table = (pt32_entry_t*)page_table_ptr;

	return page_table;
}

void map_page32(bitmap_t *bitmap, pd32_entry_t *page_dir, uint32_t virtual_addr, uint32_t physical_addr, p32_flags_t flags) {
	v_addr32_t vaddr = {.raw=virtual_addr};
	v_addr32_t physical_virt = {.raw = physical_addr};
	uint32_t page_dir_index = vaddr.parsed.pd_index;
	uint32_t page_table_index = vaddr.parsed.pt_index;
	pd32_entry_t *pd_entry = &page_dir[page_dir_index];
	if (pd_entry->present == 0) {
		pt32_entry_t* page_table = create_page_table32(bitmap);
		fill_pde32(pd_entry, 1, flags.rw, flags.us, flags.pwt, flags.pcd, 0, 0, 0, 0, (uint32_t)page_table);
	}
	pt32_entry_t *page_table = (pt32_entry_t*)pd_entry->frame;
	pt32_entry_t *pt_entry = &page_table[page_table_index];
	if (pt_entry->present == 0) {
		uint32_t page_addr = (physical_virt.parsed.pt_index | physical_virt.parsed.pd_index) << 12;
		fill_pte32(pt_entry, 1, flags.rw, flags.us, flags.pwt, flags.pcd, 0, 0, 0, 0, 0, page_addr);
	}
}

void identity_mapping32(bitmap_t *bitmap, pd32_entry_t *page_dir, p32_flags_t flags) {
	for (uint32_t i = 0;i<bitmap->memory_amount; i += bitmap->page_size) {
		map_page32(bitmap, page_dir, i, i, flags);
	}
}
