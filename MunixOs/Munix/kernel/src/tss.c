#include "../include/tss.h"


void init_tss(void) {
	kprintf("Setting Task State Segment...\n");
	memset(&_kernel_tss_segment, 0, sizeof(tss_t));
	tss_t *kernel_tss = (tss_t*)&_kernel_tss_segment;
	kernel_tss->esp0 = (int)&_kernel_tss_stack_top;
	kernel_tss->ss0 = 0x10; // 0x10 es el selector de datos del kernel
	kernel_tss->iopb = sizeof(tss_t);
	awake_tss();
	kprintf("Task State Segment Ready!\n");
}
