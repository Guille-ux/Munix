#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdbool.h>

#define SHELL_BUFFER_SIZE 256

extern char *shell_prompt;
extern char shell_buffer[SHELL_BUFFER_SIZE];
extern uint32_t shell_index;

void shell_putc(char c);
void shell_update();
int shellEntry();

#endif
