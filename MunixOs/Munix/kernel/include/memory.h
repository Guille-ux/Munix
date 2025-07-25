#ifndef MEMORY_H
#define MEMORY_H

#include "libcs2.h"
#include "zynk.h"
#include "ksysarena.h"
#include "buddy_alloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	uint8_t *pool_base;
	size_t size;
} normal_memory;

typedef struct {
	void* (*kmalloc)(size_t size);
	void* (*kfree)(void *ptr);
	void *metadata
} stdmem_t;

extern stdmem_t stdmem_interface;

void config_stdmem_sysarena(ArenaManager *manager, Arena *arenas, uint8_t *memory, size_t size, size_t num_arenas);
void config_stdmem_buddy(void *heap_start, size_t heap_size, int min_block_order, free_node ***new_free_list);

// Funciones de sysarena adaptadas, eliminan la necesidad del manager

void* sysarena_kmalloc(size_t size);
void* sysarena_kfree(void *ptr);

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

#endif
