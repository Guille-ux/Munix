#include "../include/timer.h"
#include "../include/libcs2.h"
#include "../include/pic.h"
#include "../include/idt.h"

volatile uint32_t tick_counter=0;

void timer_init(int hz) {
	uint16_t divisor = PIT_FREQ / hz;

	outb(PIT_COMMAND, 0x36);

	outb(PIT_CHANNEL0, (divisor & 0xFF));
	outb(PIT_CHANNEL0, (divisor >> 8));
	kprintf("PIT -> \n\t -> Configured to %d  Hz\n\t -> Divisor %d \n", hz, (int)divisor);
}

void timer_handler(uint32_t *esp) {
	//kprintf("Tick!\n");
	tick_counter++;
	if(tick_counter >= 0xFFFFFFFF) tick_counter = 0; // Reset para evitar overflow
	
	pic_eoi(0); // Enviar EOI para IRQ0
}
void timer_handler_2() {
	//kprintf("Tick!\n");
	tick_counter++;
	if(tick_counter >= 0xFFFFFFFF) tick_counter = 0; // Reset para evitar overflow
}

void register_timer_handler() {
	idt_set_gate(32, (uint32_t)timer_handler, 0x08, 0x8E, 0); // IRQ0 mapped to IDT entry 32
}
