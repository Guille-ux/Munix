#ifndef MUNIX_TASKS_H
#define MUNIX_TASKS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h> 

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

struct scheduler_map_entry;

typedef struct {
	uint32_t esp;
	uint32_t stack_base;
	struct scheduler_map_entry *memory;
	int pid;
	taskState status;
} task_t;

typedef struct scheduler_map_entry {
	uint32_t start;
	uint32_t end;
	uint32_t pid; // si es -1 entonces es bloque libre
	struct scheduler_map_entry *next;
	struct scheduler_map_entry *prev;
} scheduler_map_entry_t;


typedef struct {
	uint32_t region_start;
	uint32_t region_end;
	scheduler_map_entry_t *map_start;
} scheduler_map_t; // es un mapa para q el scheduler sepa q puede asignar
		   // y q no de memoria.

void initClockTask(int nibtc); 
// nibtc == Number of Interrupts Before Task Change

inline void enableClockTask() {
	clock_task.is_enabled = true;
}

inline void disableClockTask() {
	clock_task.is_enabled = false;
}

#endif
