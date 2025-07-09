#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "vga.h"

typedef struct {
	size_t (*get_max_x)(void);
	size_t (*get_max_y)(void);
	size_t (*get_cur_x)(void);
	size_t (*get_cur_y)(void);
	void (*print_string)(size_t x, size_t y, const char *str, MultiColor color);
	void (*putchar)(size_t x, size_t y, char ch, MultiColor color);
	void (*setcur)(size_t x, size_t y);
	void (*clear)(MultiColor color);
	void (*fill_line)(size_t line, vga_char character);
	void (*scroll)(void);
	bool active;
	MultiColor default_color;
} stdout;

extern stdout stdout_interface;

void stdout_init_vga();
void stdout_init_buffer();
void disable_stdout();
void enable_stdout();
void kclear();

void kchcolor(MultiColor new_color);

#endif
