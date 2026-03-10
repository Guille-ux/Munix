#include "../include/tss.h"

void init_tss(void) {
	kprintf("Setting Task State Segment...\n");
	tss_t *kernel_tss = &_kernel_tss_segment;
	kernel_tss->esp0 = &_kernel_tss_stack_top;
	kernel_tss->ss0 = 0x10; // 0x10 es el selector de datos del kernel
	kernel_tss->iopb = sizeof(kernel_tss);
	awake_tss(void);
	kprintf("Task State Segment Ready!\n");
}
