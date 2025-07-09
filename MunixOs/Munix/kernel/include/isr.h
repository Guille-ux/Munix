#ifndef ISR_H
#define ISR_H

#include "auto-generated-isr.h"
#include <stdint.h>

typedef struct {
    uint32_t ds;         // Data Segment selector 
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; // pusha 
    uint32_t int_no;     // Número de interrupción 
    uint32_t err_code;   // Código de error 
    uint32_t eip;        // Instruction Pointer 
    uint32_t cs;         // Code Segment selector
    uint32_t eflags;     // EFLAGS register 
    uint32_t user_esp;   // User Stack Pointer 
    uint32_t user_ss;    // User Stack Segment 
} __attribute__((packed)) registers_t_32bit;

// future implementation
typedef struct {
	uint64_t isr_num;
	uint64_t err_code;
	// Add More
} __attribute__((packed)) registers_t_64bit;
#ifdef _X86_64_
	typedef registers_t_64bit registers_t;
#else
	typedef registers_t_32bit registers_t;
#endif

void isr_handler(registers_t regs);
void kernel_keyboard_handler(uint32_t *esp);

#endif
