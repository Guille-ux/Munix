#include "tasks.h"
#include "../include/gdt.h"
#include "../memory/bitmap.h"

extern char _bitmap_data;

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
	// primero cambiamos la dirección base y limite de los segmentos de usuario
	task_t *t = &k_scheduler.current->task;
	ch_gate_addr(4, (uint32_t)t->main_memory, t->main_mem_len);
	ch_gate_addr(5, (uint32_t)t->main_memory, t->main_mem_len);
	// ahora copiamos los registros del proceso a ejecutar
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
	// ponemos los eflags!
	newProc->task.registers.eflags = 1 << 9;
	newProc->task.mailbox = bitmap_alloc(&(bitmap_t)_bitmap_data);
	newProc->task.mailbox.pid = newProc->task.pid;
	newProc->task.mailbox.tail = 0;
	newProc->task.mailbox.head = 0;
	newProc->task.mailbox.count = 0;
	newProc->task.mailbox.max_msg = 31;
	
	k_scheduler.start = newProc;

	return 0;
}

int getPid() {
	return k_scheduler.current->task.pid;
}

void twait() {
	k_scheduler.current->task.status = TASK_WAITING;
	// despues de una interrupción el kernel llama a otra cosa sabes?
}

task_list_t *searchPid(int pid) {
	task_list_t *cdx=k_scheduler.start;
	while (cdx->task.pid != pid) cdx = cdx->next;
	return cdx;
}

task_list_t *searchName(char *name) {
	task_list_t *cdx=k_scheduler.start;
	while (strcmp(cdx->task.name, name)!=0) cdx = cdx->next;
	return cdx;
}

void tkill(int pid) {
	if (pid == getPid()) {
		// ok, la cosa cambia un poco
		k_scheduler.current->task.status = TASK_ZOMBIE;

	} else {
		task_list_t *t = searchPid(pid);
		t->task.status = TASK_ZOMBIE;
	}
}

int getTaskPid(char *name) {
	return searchName(name)->task.pid;
}

void tawake(int pid) {
	task_list_t *cdx = searchPid(pid);
	if (cdx->task.status == TASK_WAITING) {
		cdx->task.status = TASK_READY;
	}
}

void ipc_receive(msg_t *message) {
	MailBox_t *mailbox = k_scheduler.current->task.mailbox;
	if (mailbox->count <= 0) {
		// el buffer esta vacío!
		// creo q deberiamos poner el proceso a esperar
		// hasta q llegue un mensaje
		wait();
	} else {
		memcpy(message, mailbox->buffer[mailbox->tail << 7], 128);
		mailbox->count--;
		mailbox->tail = (mailbox->tail + 1) % mailbox->max_msg;
	}
}

void ipc_send(int pid, msg_t *message) {
	MailBox_t *mailbox = searchPid(pid)->task.mailbox;
	tawake(pid);
	if (mailbox->count >= mailbox->max_msg) {
		// o q mal, no le
	} else {
		memcpy(mailbox->buffer[mailbox->head << 7], message, 128);
		mailbox->count++;
		mailbox->head = (mailbox->head+1) % mailbox->max_msg;
	}
}
