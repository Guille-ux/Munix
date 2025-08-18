#include "../include/handler.h"

static handle_chain_t g_handles = NULL;

void register_handle_chain(handle_t *handles, size_t len) {
	for (size_t i=0;i<len;i++) {
		register_handle(&handles[i]);
	}
}

void register_handle(handle_t *handle) {
	handle->next = (handle_t*)g_handles;
	g_handles = (handle_chain_t)handle;
}

void **alloc_handle() {
	if (g_handles==NULL) return NULL;
	system_handle_t *handle = (system_handle_t*)g_handles;
	g_handles = (handle_chain_t)handle->next;
	
	return (void**)((char*)handle + sizeof(system_handle_t));
}

void free_handle(void **ptr) {
	handle_t *handle = (handle_t*)((char*)ptr - sizeof(system_handle_t));
	register_handle(handle);
}
