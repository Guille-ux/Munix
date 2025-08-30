#include "multiboot_mmap.h"
#include "multiboot_def.h"

bool multiboot_mmap_scanner(bitmap_t *bitmap, void *multiboot_info) {
	mb_mmap_info_t *mb = (mb_mmap_info_t*)multiboot_info;
	size_t mmap_len = mb->mmap_len;
	multiboot_mem_map_t *mmap_addr = (multiboot_mem_map_t*)mb->mmap_addr;

	if (mb->signature!=MB_MMAP_SCANNER_SIGNATURE) return false;
	
	bitmap_set_range(bitmap, 0, bitmap->bitmap_size*8);

	for (size_t i=0;i<mmap_len;i++) {
		size_t size_of_section = mmap_addr->length;
		if (mmap_addr->type != MULTIBOOT_MEM_AVAILABLE) continue;
		size_t addr_of_section = mmap_addr->base_addr;

		size_t section_n_entries = size_of_section / bitmap->page_size;
		size_t section_index = addr_of_section / bitmap->page_size;
		bitmap_clear_range(bitmap, section_index, section_n_entries);

		mmap_addr = (multiboot_mem_map_t*)(mmap_addr->size + (size_t)mmap_addr);
	}

	return true;
}

void multiboot1_build_mmap_info(mb_mmap_info_t *mmap_info, multiboot_info_t *mbi) {
	mmap_info->signature = MB_MMAP_SCANNER_SIGNATURE;
	mmap_info->mmap_addr = mbi->mmap_addr;
	mmap_info->mmap_len = mbi->mmap_length;
}
