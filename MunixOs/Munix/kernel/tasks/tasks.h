#ifndef MUNIX_TASKS_H
#define MUNIX_TASKS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h> 
#include "../include/libcs2.h"
#include "../include/isr.h"

typedef struct {
	bool is_enabled; // el reloj necesita saber como va
	int current_count; // interrupciones de reloj desde
			   // q se hizo el último cambio de tarea
	int nibtc; // nº de interrupciones de reloj antes
			// de volver a hacer un cambio de tarea
} multitask_clock_t; // es una estructura con estados del multitask
		     // necesarios para el reloj

extern multitask_clock_t clock_task;

typedef enum {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_FINNISH, // la tarea es finlandesa una vez q acaba
	TASK_ZOMBIE, // si no cierra correctamente
} taskState;

struct pma; // physical memory access

typedef struct pma {
	size_t start;
	size_t length;
	struct pma *next; 
} pma_t;

typedef struct {
	char *name;
	registers_t registers;
	uint32_t stack_base;
	void *main_memory;
	uint32_t main_mem_len;
	pma_t *registered_mem;
	int pid;
	taskState status;
} task_t;

struct task_list;

typedef struct task_list {
	task_t task;
	struct task_list *next;
} task_list_t;

typedef struct {
	task_list_t *start;
	task_list_t *current;
} scheduler_t;

void initClockTask(int nibtc); 
// nibtc == Number of Interrupts Before Task Change

inline void enableClockTask() {
	clock_task.is_enabled = true;
}

inline void disableClockTask() {
	clock_task.is_enabled = false;
}

extern scheduler_t k_scheduler;

void initKernelScheduler(void);
registers_t *kernel_scheduler(registers_t *regs);

#endif
