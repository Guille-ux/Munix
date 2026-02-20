#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint32_t esp;
	void *stack_base;
} thread_t;

typedef struct {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
} thread_ctx_t;

extern void save_ctx(void);
extern void restore_ctx(uint32_t esp);

#endif
