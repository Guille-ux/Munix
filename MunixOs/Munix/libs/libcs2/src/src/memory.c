#include "../include/memory.h"
#include <stddef.h>
#include <stdint.h>

malloc_t malloc=NULL;
free_t free=NULL;

void libcs_mem_init(malloc_t new_malloc, free_t new_free) {
	malloc = new_malloc;
	free = new_free;
}
