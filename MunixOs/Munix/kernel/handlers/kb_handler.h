#ifndef MUNIX_KB_HANDLER_H
#define MUNIX_KB_HANDLER_H

#define KB_BUFFER_SIZE 512

extern uint8_t kb_ring_buffer[KB_BUFFER_SIZE];
extern size_t tail;
extern size_t head;

typedef struct {
	char *name;
	void (*push)(void);
} kb_handler_t;


char buffer_pop();
void ps2_get_scancode();
void init_ps2_handler(kb_handler_t *kb_handler);

#endif
