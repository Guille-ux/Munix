#include "../include/libcs2.h"
#include "../include/idt.h"

IDT_Pointer idt_ptr;
IDT_Entry IDT_Table[256];

void idt_set_gate(uint8_t num, uintptr_t base, uint16_t selector, uint8_t type_attr, uint8_t ist_index) {
#ifdef _X86_64_
	// 64 bits logic
	IDT_Table[num].offset_1 = (uint16_t)(base & 0xFFFF);
    	IDT_Table[num].selector = selector;
    	IDT_Table[num].ist = ist_index & 0x7; 
    	IDT_Table[num].type_attr = type_attr;
    	IDT_Table[num].offset_2 = (uint16_t)((base >> 16) & 0xFFFF);
	IDT_Table[num].offset_3 = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    	IDT_Table[num].zero = 0;
#else
	// 32 bits logic
    	IDT_Table[num].offset_low = (uint16_t)(base & 0xFFFF);
	IDT_Table[num].selector = selector;
	IDT_Table[num].zero = 0;
	IDT_Table[num].type_attr = type_attr;
	IDT_Table[num].offset_high = (uint16_t)((base >> 16) & 0xFFFF);
#endif
}

void idt_init() {
	idt_ptr.limit=sizeof(IDT_Entry)*256-1;
	idt_ptr.base = (uintptr_t)&IDT_Table;
	// IDT initialization code
	
#ifdef _X86_64_
	idt_load64(&idt_ptr);
#else
	idt_load32(&idt_ptr);
#endif

	kprintf("IDT Initialized!\n");
}
