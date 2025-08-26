#include "../include/libcs2.h"
#include "bitmap.h"

void register_mmap_scanner(bitmap_t *bitmap, mmap_scanner scanner) {
	if (bitmap->n_scanners < MAX_MMAP_SCANNERS) {
		bitmap->scanners[bitmap->n_scanners++]=scanner;
	}
}

bool bitmap_scan_mmap(bitmap_t *bitmap, void *mmap) {
	if (bitmap==NULL || mmap == NULL) return false;
	bool ret=false;
	for (int i=0;i<bitmap->n_scanners;i++) {
		if (bitmap->scanners[i]==NULL) return false;
		ret = bitmap->scanners[i](bitmap, mmap);
		if (ret) break;
	}
	protect_bitmap(bitmap);
	return ret;
}

bool bitmap_init(bitmap_t *bitmap, void *bitmap_start, size_t memory_amount, size_t page_size) {
	if (bitmap_start==NULL ||
	    bitmap==NULL ||
	    memory_amount == 0 ||
	    page_size & 1) {
		return false;
	}
	memset(bitmap, 0, sizeof(bitmap_t));
	bitmap->bitmap_start = bitmap_start;
	bitmap->memory_amount = memory_amount;
	bitmap->bitmap_size = (memory_amount / page_size) / 8;
	bitmap->page_size = page_size;
	bitmap->n_scanners = 0;
	memset(bitmap_start, 0, bitmap->bitmap_size);
	return true;
}

static uint8_t get_offset(size_t index) {
	return index % 8;
}

static size_t get_index(bitmap_t *bitmap, void *page) {
	return ((size_t)page) / bitmap->page_size;
}

static int get_bit(bitmap_t *bitmap, size_t index) {
	if (bitmap->bitmap_size < (index/8)) return -1;
	uint8_t *map = bitmap->bitmap_start;
	uint8_t offset = get_offset(index);
	size_t high_index = index / 8;
	int ret = (map[high_index] & (1 << offset)) ? 1 : 0;
	return ret;
}

bool bitmap_clear_bit(bitmap_t *bitmap, size_t index) {
	if (bitmap->bitmap_size < (index/8)) return false;
	uint8_t *map = bitmap->bitmap_start;
	uint8_t offset = get_offset(index);
	size_t high_index = index / 8;
	uint8_t tmp = map[high_index];
	tmp &= (~(1 << offset));
	map[high_index] = tmp;
	return true;
}

bool bitmap_set_bit(bitmap_t *bitmap, size_t index) {
	if (bitmap->bitmap_size < (index/8)) return false;
	uint8_t *map = bitmap->bitmap_start;
	uint8_t offset = get_offset(index);
	size_t high_index = index / 8;
	uint8_t tmp = map[high_index];
	tmp |= (1 << offset);
	map[high_index] = tmp;
	return true;
}

void *bitmap_alloc(bitmap_t *bitmap) {
	size_t n_entries = bitmap->bitmap_size*8;
	size_t index = 0;
	bool found = false;
	for (size_t i=0;i<n_entries;i++) {
		if (get_bit(bitmap, i) == 0) {
			index = i;
			bitmap_set_bit(bitmap, i);
			found = true;
			break;
		}
	}
	if (!found) return ((void*)(uint32_t)-1);
	
	return (void*)(index*bitmap->page_size);
}

void bitmap_free(bitmap_t *bitmap, void *page) {
	size_t index = (size_t)(((size_t)page)/bitmap->page_size);
	bitmap_clear_bit(bitmap, index);
}

void bitmap_set_range(bitmap_t *bitmap, size_t start, size_t len) {
	for (size_t i=start;i<len;i++) {
		bitmap_set_bit(bitmap, i);
	}
}

void bitmap_clear_range(bitmap_t *bitmap, size_t start, size_t len) {
	for (size_t i=start;i<len;i++) {
		bitmap_clear_bit(bitmap, i);
	}
}

void protect_bitmap(bitmap_t *bitmap) {
	size_t start=((size_t)bitmap->bitmap_start)/bitmap->page_size;
	size_t len=bitmap->bitmap_size*8;
	bitmap_set_range(bitmap, start, len);
	bitmap_set_bit(bitmap, ((size_t)bitmap)/bitmap->page_size);
}
