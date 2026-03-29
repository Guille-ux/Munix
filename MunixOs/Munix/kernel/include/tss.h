#ifndef MUNIXOS_TSS_H
#define MUNIXOS_TSS_H

#include <stdint.h>
#include "libcs2.h"

typedef struct {
	uint16_t link;
	uint16_t reserved_0;
	uint32_t esp0;
	uint16_t ss0;
	uint16_t reserved_1;
	uint32_t esp1;
	uint16_t ss1;
	uint16_t reserved_2;
	uint32_t esp2;
	uint16_t ss2;
	uint16_t reserved_3;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es;
	uint16_t reserved_4;
	uint16_t cs;
	uint16_t reserved_5;
	uint16_t ss;
	uint16_t reserved_6;
	uint16_t ds;
	uint16_t reserved_7;
	uint16_t fs;
	uint16_t reserved_8;
	uint16_t gs;
	uint16_t reserved_9;
	uint16_t ldtr;
	uint16_t reserved_10;
	uint16_t reserved_11;
	uint16_t iopb;
	uint32_t ssp;
} __attribute__((packed)) tss_t;


extern char _kernel_tss_segment;
extern char _kernel_tss_stack_top;

extern void awake_tss(void);

void init_tss(void);

#endif
