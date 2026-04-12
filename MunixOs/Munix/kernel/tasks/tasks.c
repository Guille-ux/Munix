#include "tasks.h"

multitask_clock_t clock_task;
scheduler_t k_scheduler;

void initClockTask(int nibtc) {
	clock_task.current_count = 0;
	clock_task.nibtc = nibtc;
	clock_task.is_enabled = false;
}

void initKernelScheduler() {
	// de momento nada, ya ire añadiendo más cosas
	kprintf("Kernel Scheduler Initialized!\n");
}


int g_pid=0;

// es imposible de entender el código
// voy a escribir más comentarios
registers_t *kernel_scheduler(registers_t *regs) {
	if (k_scheduler.start == NULL) return regs; // wtf, no hay tareas
	if (k_scheduler.current->next == NULL) { // si llegamos al final de la lista
		// guardamos todo y cambiamos a la tarea del inicio
		memcpy((void*)&k_scheduler.current.task.registers, regs, sizeof(registers_t));
		k_scheduler.current.task.status = TASK_READY;
		k_scheduler.current = k_scheduler.start;
	} else {
		// guardamos todo y cambiamos a la siguiente
		memcpy((void*)&k_scheduler.current.task.registers, regs, sizeof(registers_t));
		k_scheduler.current.task.status = TASK_READY;
		k_scheduler.current = k_scheduler.current->next;
	}
	while (k_scheduler.current.task.status != TASK_READY) { // hasta q sea
								// uno
								// q podamos
								// correr
		if (k_scheduler.current->next == NULL) { // para dar la vuelta
			k_scheduler.current = k_scheduler.start;
			continue;
		}
		k_scheduler.current = k_scheduler.current->next;
	}
	// aqui hay q a;adir codigo q cambie la base y limite
	// del segmento en uso
	memcpy((void*)regs, &k_scheduler.current.task.registers, sizeof(registers_t));
	k_scheduler.current.task.status = TASK_RUNNING;
	return regs;
}

int spawnProccess(uint16_t cs, uint16_t ds, void *mem_start, uint32_t mem_amount, uint32_t eip, char *name) {
	// primero crear una entrada nueva, usando malloc
	task_list_t *newProc = malloc(sizeof(task_list_t));
	newProc->next = k_scheduler.start;
	newProc->task.name = name; // chapuzas pero temporal
	newProc->task.pid = g_pid++; // otra chapuza temporal
	newProc->task.main_memory = mem_start;
	newProc->task.main_mem_len = mem_amount;
	newProc->task.status = TASK_READY;
	newProc->task.registers.eip = eip;
	newProc->task.registers.cs = cs;
	newProc->task.registers.ds = ds;
	newProc->task.registers.fs = KERNEL_FAR_PTR;

	k_scheduler.start = newProc;

	return 0;
}
