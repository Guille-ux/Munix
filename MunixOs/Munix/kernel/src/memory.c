#include "../include/ksysarena.h"
#include "../include/buddy_alloc.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


stdmem_t stdmem_interface;

void config_stdmem_sysarena(ArenaManager *manager, Arena *arenas, uint8_t *memory, size_t size, size_t num_arenas) {
	stdmem_interface.kmalloc=sysarena_kmalloc;
	stdmem_interface.kfree=sysarena_kfree;
	if (!ksysarena_init(manager, memory, arenas, size, num_arenas)) {
		stdlog_interface.append("ERROR IN SYSARENA INITIALIZATION!");
	} else {
		stdmem_interface.metadata=(void*)manager;
		kprintf("Sysarena Initialized!\n");
	}
}

void *sysarena_kmalloc(size_t size) {
	return ksysarena_alloc((ArenaManager*)stdmem_interface.metadata, size);
}

void *sysarena_kfree(void *ptr) {
	ksysarena_free((ArenaManager*)stdmem_interface.metadata, ptr);
	return NULL;
}

void *buddy_kfree(void *ptr) {
	buddy_free(ptr);
	return NULL;
}

void config_stdmem_buddy(void *heap_start, size_t heap_size, int min_block_order, free_node ***new_free_list) {
	stdmem_interface.kmalloc=buddy_malloc;
	stdmem_interface.kfree=buddy_kfree;
	buddy_init(heap_start, heap_size, min_block_order, new_free_list);
}