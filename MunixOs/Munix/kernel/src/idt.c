#include "../include/libcs2.h"
#include "../include/idt.h"
#include "../include/auto-generated-isr.h"
#include "../include/pic.h"
#include "../include/timer.h"
#include "../include/isr.h"

#include <stdint.h>

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

	kprintf("Installing IDT...\n");

	// idt_set_gate(number, (uint32_t)isr_stub, 0x08, 0x8E);

	idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E, 0);
	kprintf("Zero Division Installed\n");

	idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E, 0);
	kprintf("Invalid Opcode Installed\n");

	idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E, 0);
	kprintf("Double Fault Installed\n");

	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E, 0);
	kprintf("General Protection Fault Installed\n");

	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E, 0);
	kprintf("Page Fault Installed\n");

	kprintf("Essential Descriptors Installed\n");

	//idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E, 0);
	register_timer_handler(); 
	kprintf("IRQ0 (timer) Descriptor Installed\n");

	idt_set_gate(0x21, (uint32_t)kernel_keyboard_handler, 0x08, 0x8E, 0);
	kprintf("IRQ1 (keyboard) Descriptor Installed\n");

	kprintf("IRQ Descriptors Installed\n");


	pic_remap(0x20, 0x28);

	pic_unmask_irq(0);
	pic_unmask_irq(1);

#ifdef _X86_64_
	idt_load64(&idt_ptr);
#else
	idt_load32(&idt_ptr);
#endif

	kprintf("IDT Initialized!\n");

	__asm__ volatile("sti");
}
