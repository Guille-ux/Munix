#ifndef GDT_H
#define GDT_H

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) GDT_Entry;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) GDT_Pointer;

void gdt_init(void);
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

extern void gdt_flush(void);

#endif
