#include "kb_handler.h"
#include "../include/libcs2.h"

uint16_t kb_ring_buffer[KB_BUFFER_SIZE];
size_t tail=0;
size_t head=0;

// un global
kb_handler_t ps2_handler;

// ESTADOS INTERNOS
char codebreak;

const char ps2_kb_name[] = "MUNIX PS2 HANDLER";

uint16_t buffer_pop() {
	if (tail == head) {
		return '\0';
	}
	uint16_t ch = kb_ring_buffer[tail];
	tail = (tail + 1) % KB_BUFFER_SIZE;
	return ch;
}

void ps2_get_scancode() {
	uint8_t scancode = inb(0x60); // leemos el puerto del teclado
	if (scancode == 0xE0) extended=1; return;

	uint16_t final_sc = scancode | (extended << 8);
	buffer_push(final_sc);
	extended=0;
	codebreak=0;
}

void buffer_push(uint16_t item) {
	if ((head + 1) % KB_BUFFER_SIZE == tail) {
		return; // signifíca que esta lleno
	}
	kb_ring_buffer[head] = item;
	head = (head + 1) % KB_BUFFER_SIZE;	
}

void init_ps2_handler(kb_handler_t *kb_handler) {
	kb_handler->name = ps2_kb_name;
	kb_handler->getch = ps2_get_scancode;
}
