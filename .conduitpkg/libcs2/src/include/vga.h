#ifndef VGA_H
#define VGA_H

#include "display.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VGA_TEXT_MODE_80x25_ID 0x03

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


void vga_init(uint8_t mode);
void vga_clear(MultiColor color);
void vga_putchar(uint16_t x, uint16_t y, char c, uint8_t attr);
void vga_print_string(uint16_t x, uint16_t y, const char *str, uint8_t attr);
void vga_setcur(uint16_t x, uint16_t y);
void vga_putpix(uint16_t x, uint16_t y, rgb color);
void vga_setmode(uint8_t new_mode);
void vga_scroll();
void vga_fill_line(uint16_t line, vga_char character);
#endif
