#ifndef HANDLER
#define HANDLER

#include <stdint.h>
#include <stddef.h>

struct handle_t;
struct system_handle_t;

typedef struct handle_t {
	struct handle_t *next;
	void *ptr;
} handle_t;

typedef struct system_handle_t {
	struct system_handle_t *next;
} system_handle_t;

typedef void* usr_handle_t;
typedef system_handle_t *handle_meta;

typedef handle_t* handle_chain_t;

void register_handle_chain(handle_t *handles, size_t len);
void register_handle(handle_t *handle);
void **alloc_handle();
void free_handle(void **ptr);

#endif
