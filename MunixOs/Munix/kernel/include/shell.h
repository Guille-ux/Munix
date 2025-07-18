#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdbool.h>

#define SHELL_BUFFER_SIZE 256

extern bool send;
extern bool backspace;
extern bool shell_event;
extern char *shell_prompt;
extern char shell_buffer[SHELL_BUFFER_SIZE];
extern uint32_t shell_index;

void push_to_buffer(char c);
void pop_from_buffer();
void shell_update();
int shellEntry();

#endif
