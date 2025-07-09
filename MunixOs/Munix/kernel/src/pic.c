#include "../include/libcs2.h"
#include "../include/pic.h"

void pic_remap(uint8_t offset1, uint8_t offset2) {
	kprintf("PIC -> Remapping IRQs to \n\t -> Master: %d \n\t -> Slave: %d \n", offset1, offset2);

	uint8_t a1 = inb(PIC1_DATA);
	uint8_t a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_IC4); // start Master PIC with ICW4
	outb_delay();


	outb(PIC2_COMMAND, ICW1_INIT | ICW1_IC4); // doing the same with Slave PIC
	outb_delay();

	outb(PIC1_DATA, (uint8_t)offset1);
	outb_delay();

	outb(PIC2_DATA, (uint8_t)offset2);
	outb_delay();

	outb(PIC1_DATA, 0x04);
	outb_delay();

	outb(PIC2_DATA, 0x02);
	outb_delay();

	outb(PIC1_DATA, ICW4_8086);
	outb_delay();
	
	outb(PIC2_DATA, ICW4_8086);
	outb_delay();
}
void pic_enable(void) {
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}
void pic_disable(void) {
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
}

void pic_eoi(uint8_t irq) {
	if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

void pic_mask_irq(uint8_t irq_line) {
	uint16_t port;
	uint8_t value;
	if (irq_line < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) | (1 << irq_line);
	outb(port, value);
}

void pic_unmask_irq(uint8_t irq_line) {
	uint16_t port;
	uint8_t value;
	if (irq_line < 8) { 
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) & ~(1 << irq_line);
	outb(port, value);
}
