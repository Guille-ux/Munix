#include "kb_handler.h"
#include "../include/libcs2.h"

uint8_t kb_ring_buffer[KB_BUFFER_SIZE];
size_t tail=0;
size_t head=0;

const char ps2_kb_name[] = "MUNIX PS2 HANDLER";

char buffer_pop() {
	if (tail == head) {
		return '\0';
	}
	char ch = kb_ring_buffer[tail];
	tail = (tail + 1) % KB_BUFFER_SIZE;
	return ch;
}

void ps2_get_scancode() {
	if ((head + 1) % KB_BUFFER_SIZE == tail) {
		return; // signifíca que esta lleno
	}
	char ch = inb(0x60);
	kb_ring_buffer[head] = ch;
	head = (head + 1) % KB_BUFFER_SIZE;
}

void init_ps2_handler(kb_handler_t *kb_handler) {
	kb_handler->name = ps2_kb_name;
	kb_handler->push = ps2_get_scancode;
}
