#include "../include/libcs2.h"
#include "../include/gdt.h"

#define GDT_LEN 3

static GDT_Entry gdt_table[GDT_LEN];

GDT_Pointer gdt_ptr;

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

	gdt_flush();

	kprintf("GDT Initialized and Loaded\n");

}
