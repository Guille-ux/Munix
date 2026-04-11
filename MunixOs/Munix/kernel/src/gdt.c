#include "../include/libcs2.h"
#include "../include/gdt.h"
#include "../include/tss.h"

#define GDT_LEN 5

static GDT_Entry gdt_table[GDT_LEN];

GDT_Pointer gdt_ptr;

void ch_gate_addr(int num, uint32_t base, uint32_t limit) {
	gdt_table[num].base_low = (base & 0xFFFF);
	gdt_table[num].base_middle = (base >> 16) & 0xFF;
	gdt_table[num].base_high = (base >> 24) & 0xFF;

	gdt_table[num].limit_low = (limit & 0xFFFF);
	
	gdt_table[num].granularity &= 0xF0;

	gdt_table[num].granularity |= ((limit >> 16) & 0x0F);	
}

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
	// GDT Table
	gdt_table[num].base_low = (base & 0xFFFF);
	gdt_table[num].base_middle = (base >> 16) & 0xFF;
	gdt_table[num].base_high = (base >> 24) & 0xFF;

	gdt_table[num].limit_low = (limit & 0xFFFF);
	
	gdt_table[num].granularity = ((limit >> 16) & 0x0F);

	gdt_table[num].granularity |= (gran & 0xF0);

	gdt_table[num].access = access;
}

extern char _kernel_tss_segment;
extern char _kernel_end;

void gdt_init(void) {
	kprintf("Adding GDT Entries...\n");
	gdt_ptr.limit = (sizeof(GDT_Entry)*GDT_LEN)-1;
	
	gdt_ptr.base = (uint32_t)&gdt_table;

	gdt_set_gate(0, 0, 0, 0, 0); // Necesario para x86
	kprintf("Null Descriptor Added\n");

	gdt_set_gate(1, 0x0, 0xFFFFFFFF, 0x9A, 0xCF);
	kprintf("Kernel Code Descriptor Added\n");

	gdt_set_gate(2, 0x0, 0xFFFFFFFF, 0x92, 0xCF);
	kprintf("Kernel Data Descriptor Added\n");

	gdt_set_gate(3, (uint32_t)&_kernel_tss_segment, sizeof(tss_t)-1, 0x89, 0x00);
	kprintf("Kernel Task State Segment Descriptor Added\n");

	gdt_set_gate(4, (uint32_t)&_kernel_end, 0xFFFFFFFF, 0b11111010, 0b11000000);
	kprintf("User Code Descriptor Added\n");

	gdt_set_gate(5, (uint32_t)&_kernel_end, 0xFFFFFFFF, 0b11110010, 0b11000000);
	kprintf("User Data Descriptor Added\n");

	gdt_set_gate(6, 0, 0xFFFFFFFF, 0b11110010, 0b11000000);
	
	kprintf("User Far Segment Descriptor Added\n");

	gdt_flush();

	kprintf("GDT Initialized and Loaded\n");

}
