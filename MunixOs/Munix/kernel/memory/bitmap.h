#infdef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


struct bitmap_mng;

#define MAX_MMAP_SCANNERS 2

typedef bool (*mmap_scanner)(struct bitmap_mng *bitmap, void *mmap);

typedef struct bitmap_mng {
	void *bitmap_start;
	size_t memory_amount;
	size_t bitmap_size;
	size_t page_size;
	mmap_scanner scanners[MAX_MMAP_SCANNERS];
	int n_scanners;
} bitmap_t;

bool bitmap_init(bitmap_t *bitmap, void *bitmap_start, size_t memory_amount, size_t page_size);

void *bitmap_alloc(bitmap_t *bitmap);
void bitmap_free(bitmap_t *bitmap, void *page);

void register_mmap_scanner(bitmap_t *bitmap, mmap_scanner scanner);
bool bitmap_scan_mmap(bitmap_t *bitmap, void *mmap);

bool bitmap_clear_bit(bitmap_t *bitmap, size_t index);
bool bitmap_set_bit(bitmap_t *bitmap, size_t index);

void protect_bitmap(bitmap_t *bitmap);

void bitmap_set_range(bitmap_t *bitmap, size_t start, size_t len);
void bitmap_clear_range(bitmap_t *bitmap, size_t start, size_t len);

#endif
