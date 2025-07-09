#ifndef STDBUFF
#define STDBUFF
// este archivo define STDBUFF un sistema de un buffer para no usar VGA directamente, servira para cuando haya modo gráfico y para testear algunas partes del kernel

#include "display.h"
#include "vga.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern MultiColor *color_buffer;
extern char *buffer;
extern size_t buff_height;
extern size_t buff_width;
extern size_t buff_cur_x;
extern size_t buff_cur_y;

// actualmente se usan char's el objetivo es pasar a UNICODE, o a archivos de fuentes a secas

void buff_init(char *buff, MultiColor *color_buff, size_t height, size_t width);
void buff_setcur(size_t x, size_t y);
void buff_print_string(size_t x, size_t y, const char *str, MultiColor color);
void buff_putchar(size_t x, size_t y, char c, MultiColor color);
void buff_clear(MultiColor color);
void buff_scroll();
void buff_fill_line(size_t line, vga_char character);
size_t buff_get_max_x();
size_t buff_get_max_y();
size_t buff_get_cur_x();
size_t buff_get_cur_y();


#endif
