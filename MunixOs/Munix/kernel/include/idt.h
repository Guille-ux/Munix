#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero; // always 0
	uint8_t type_attr; // gate type, dpl, and 'p'
	uint16_t offset_high;
} __attribute__((packed)) IDT32_Entry;

typedef struct {
	uint16_t offset_1;
	uint16_t selector;
	uint8_t ist; // 0 to 2 Interrupt Stack Tabke offset
	uint8_t type_attr;
	uint16_t offset_2;
	uint32_t offset_3;
	uint32_t zero;
} __attribute__((packed)) IDT64_Entry;

#ifdef _X86_64_
	typedef IDT64_Entry IDT_Entry;
#else
	typedef IDT32_Entry IDT_Entry;
#endif

extern IDT_Entry IDT_Table[256];

typedef struct {
	uint16_t limit;
#ifdef _X86_64_
	uint64_t base;
#else
	uint32_t base;
#endif
} __attribute__((packed)) IDT_Pointer;

void idt_set_gate(uint8_t num, uintptr_t base, uint16_t selector, uint8_t type_attr, uint8_t ist_index);

void idt_init();

extern void idt_load32(IDT_Pointer *idt_ptr);
extern void idt_load64(IDT_Pointer *idt_ptr);

#endif
