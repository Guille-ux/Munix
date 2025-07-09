#include "../include/ksysarena.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include <stddef.h>
#include <stdint.h>


stdmem_t stdmem_interface;

void config_stdmem_sysarena(ArenaManager *manager, Arena *arenas, uint8_t *memory, size_t size, size_t num_arenas) {
	stdmem_interface.kmalloc=sysarena_kmalloc;
	stdmem_interface.kfree=sysarena_kfree;
	if (!ksysarena_init(manager, memory, arenas, size, num_arenas)) {
		stdlog_interface.append("ERROR IN SYSARENA INITIALIZATION!");
	} else {
		stdmem_interface.manager=manager;
		kprintf("Sysarena Initialized!\n");
	}
}

void *sysarena_kmalloc(size_t size) {
	return ksysarena_alloc(stdmem_interface.manager, size);
}

void *sysarena_kfree(void *ptr) {
	ksysarena_free(stdmem_interface.manager, ptr);
	return NULL;
}
