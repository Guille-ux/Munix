#include "munx.h"

void munxInit(void *bin_start, uint32_t min_ram, uint32_t preferred_ram, uint32_t entry_point) {
	munx_header_t *header = (munx_header_t*)bin_start;
	header->min_ram = min_ram;
	header->preferred_ram = preferred_ram;
	header->entry_point = entry_point;
	header->magic = MUNX_MAGIC;
}
