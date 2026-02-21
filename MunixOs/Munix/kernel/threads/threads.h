#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
	STATUS_ZOMBIE,
	STATUS_WAITING,
	STATUS_RUNNING,
	STATUS_FINNISH, // un pequeño guiño a linus 
			// (en vez de acabado, finlandes)
	STATUS_READY,
	STATUS_NULL, // es un hilo vacio
} proc_status_t;

typedef struct {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t old_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;	
} thread_ctx_t;

typedef struct {
	thread_ctx_t ctx;
	void *stack_base;
	uint32_t stack_size;
} thread_t;

typedef struct {
	thread_t thread;
	proc_status_t status;	
	int pid;
} process_t;

#define MAX_PROCESSES 10 // cambialo para los procesos q vayas a necesitar, 
			 // yo pongo un límite para evitar usar 
			 // funciones externas como malloc y free

extern process_t proc_list[MAX_PROCESSES];

extern void save_ctx(void) __attribute__((cdecl));
extern void restore_ctx(uint32_t esp) __attribute__((cdecl));

void scheduler(uint32_t esp) __attribute__((cdecl));
void init_proc_list(void);
process_t *find_free_proc(void);
void spawn(void *(entry_point)(), void *stack_base, uint32_t stack_size);

#define yield() (save_ctx())

#endif
