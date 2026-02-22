#include "threads.h"

process_t proc_list[MAX_PROCESSES];
process_t *current_proc = NULL;

void scheduler(uint32_t esp) {
	// guardar el proceso, ah si, se llamara a una función especial
	// si el proceso acabó
	current_proc->thread.ctx = *((void*)esp);
	current_proc->status = STATUS_READY;

	// buscar nuevo proceso
	int pid=current_proc->pid;
	process_t *newProc = NULL;
	while (1) {
		pid = (pid + 1) % MAX_PROCESSES;
		if (proc_list[pid].status == STATUS_READY) {
			newProc = &proc_list[pid];
			break;
		}
	}
	if (newProc == NULL) {
		// ERROR, no hay más procesos
	}
	current_proc = newProc;
	current_proc->status = STATUS_RUNNING;

	restore_ctx(current_proc->thread.ctx.old_esp);
}

void init_proc_list() {
	for (int i=0;i<MAX_PROCESSES;i++) {
		proc_list[i].status = STATUS_NULL;
		proc_list[i].pid = i;
	}
}

process_t *find_free_proc() {
	for (int i=0;i<MAX_PROCESSES;i++) {
		if (proc_list[i].status == STATUS_NULL) {
			return &proc_list[i];
		}
	}
	return NULL;
}

process_t *find_zombie_or_finnish() {
	for (int i=0;i<MAX_PROCESSES;i++) {
		if (proc_list[i].status == STATUS_ZOMBIE ||
		    proc_list[i].status == STATUS_FINNISH) {
			return &proc_list[i];
		}
	}
	return NULL;
}


int spawn(void (*entry_point)(), void *stack_base, uint32_t stack_size) {
	process_t *newProc = find_free_proc();
	if (!newProc) {
		return -1;
	}
	newProc->status = STATUS_READY;
	*(uint32_t*)stack_base = (uint32_t)entry_point;
	(uint32_t)stack_base -= sizeof(uint32_t);
	newProc->thread.stack_base = stack_base;
	newProc->thread.stack_size = stack_size;
	newProc->thread.ctx.old_esp = (void*)((uint32_t)stack_base - sizeof(uint32_t));
	newProc->thread.ctx.ebp = stack_base;
	thread_ctx_t *initial_ctx = (void*)((uint32_t)stack_base - sizeof(thread_ctx_t)); 
	*initial_ctx = newProc->thread.ctx;
	return 0;
}

int get_pid(void) {
	if (!current_proc) {
		// ERROR!
		// no hay un proceso actual
		return -1;
	}
	return current_proc->pid;
}

int revive(void *(entry_point)()) {
	process_t *newProc = find_zombie_or_finnish();
	if (!newProc) {
		return -1;
	}
	newProc->status = STATUS_READY;	
	newProc->thread.stack_base = (void*)((uint32_t)newProc->thread.stack_base - sizeof(uint32_t));
	*(uint32_t*)newProc->thread.stack_base = (uint32_t)entry_point;	
	newProc->thread.ctx.old_esp = (void*)((uint32_t)newProc->thread.stack_base - sizeof(uint32_t));
	newProc->thread.ctx.ebp = stack_base;
	thread_ctx_t *initial_ctx = (void*)((uint32_t)newProc->thread.stack_base - sizeof(thread_ctx_t)); 
	*initial_ctx = newProc->thread.ctx;
	return 0;
}
