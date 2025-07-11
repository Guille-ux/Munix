#ifndef LIBCS_MEMORY
#define LIBCS_MEMORY

#include <stddef.h>
#include <stdint.h>

typedef void* (*malloc_t)(size_t size);
typedef void* (*free_t)(void *ptr);

extern malloc_t malloc;
extern free_t free;

void libcs_mem_init(malloc_t new_malloc, free_t new_free);

#endif
