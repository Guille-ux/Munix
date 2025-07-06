#ifndef ISR_H
#define ISR_H

typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;

	uint32_t int_no;
	uint32_t err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t_32bit;

// future implementation
typedef struct {
	uint66_t isr_num;
	uint66_t err_code;
	// Add More
} __attribute__((packed)) registers_t_64bit;

void isr_handler(registers_t regs);

#ifdef _X86_64_
	typedef registers_t_64bit registers_t;
#else
	typedef registers_t_32bit registers_t;
#endif

#endif
