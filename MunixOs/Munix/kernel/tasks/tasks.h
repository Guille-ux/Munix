#ifndef MUNIX_TASKS_H
#define MUNIX_TASKS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h> 
#include "../include/libcs2.h"
#include "../include/isr.h"
#include "../fs/fsd.h"
#include "../usr/usr.h"

#define KERNEL_FAR_PTR 48
#define MAX_FILE_DESCRIPTORS 32

// TODO: AÑADIR FREE_PROC, las syscalls (TODAS)


typedef struct {
	int pid;
	uint8_t data[124];
} __attribute__((packed)) msg_t;

typedef struct {
	int pid;
	uint32_t tail;
	uint32_t head;
	uint32_t count;
	uint32_t max_msg;
	uint8_t padding[104];
	uint8_t buffer[3968]
} __attribute__((packed)) MailBox_t;

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
	TASK_NULL=0,
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_FINNISH, // la tarea es finlandesa una vez q acaba
	TASK_ZOMBIE, // si no cierra correctamente
} taskState;

struct pma; // physical memory access

typedef struct pma {
	size_t start;
	size_t length; // in pages
} pma_t;

#define MAX_REGIONS 32
#define MAX_FDS 32
#define MAX_MSG 31

typedef struct {
	char *name;
	registers_t registers;
	//uint32_t stack_base;
	void *main_memory;
	uint32_t main_mem_len; // in bytes
	pma_t registered_mem[MAX_REGIONS]; // me parece q es muy probable q no lo use
	MailBox_t *mailbox;
	int pid;
	taskState status;
	int file_descriptors[MAX_FDS]; // es un array de fid's
	kid_t kid;
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
int spawnProccess(uint16_t cs, uint16_t ds, void *mem_start, uint32_t mem_amount, uint32_t eip, char *name);

int getPid(void);
void twait(void);
void tkill(int pid);
task_list_t *searchPid(int pid);
task_list_t *searchName(char *name);
int getTaskPid(char *name);
void tawake(int pid);
void ipc_receive(msg_t *message);
void ipc_send(int pid, msg_t *message);
uint32_t sys_whoami();
int freeProc(int pid);
int sys_spawn(uint32_t ram_amount, void *blob, uint32_t length, uint32_t start_pos);

#endif
