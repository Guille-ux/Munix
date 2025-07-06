#ifndef VGA_H
#define VGA_H

#include "display.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VGA_TEXT_MODE_80x25_ID 0x03

extern uint16_t vga_height;
extern uint16_t vga_width;
extern uint16_t vga_global_x;
extern uint16_t vga_global_y;
extern uint8_t current_mode;
extern uint16_t vga_cur_x;
extern uint16_t vga_cur_y;

typedef struct vga_char {
	uint8_t character;
	uint8_t attr; // (background << 4) | foreground (in little endian)
} __attribute__((packed)) vga_char;

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14, // aka Yellow
    VGA_COLOR_WHITE = 15,
};

typedef enum vga_color vga_color;


void vga_init();
void vga_clear(MultiColor color);
void vga_putchar(size_t x, size_t y, char c, MultiColor color);
void vga_print_string(size_t x, size_t y, const char *str, MultiColor color);
void vga_setcur(size_t x, size_t y);
void vga_start();
void vga_disable();
void vga_enable();
void vga_scroll();
void vga_fill_line(size_t line, vga_char character);
void dump_vga_registers_minimal_read_only();
size_t vga_get_max_x();
size_t vga_get_max_y();
size_t vga_get_cur_x();
size_t vga_get_cur_y();

#endif
