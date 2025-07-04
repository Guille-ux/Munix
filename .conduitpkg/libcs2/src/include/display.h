#ifndef DISPLAY
#define DISPLAY

#include <stdint.h>

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} /*__attribute__((packed)) */ rgb;

/*
 *
typedef struct {
	void (*init)(uint8_t mode);
	void (*clear)(void);
	void (*put_char)(uint16_t x, uint16_t y, char c, uint8_t attr);
	void (*print_string)(uint16_t x, uint16_t y, const char *str, uint8_t attr);
	void (*set_cursor)(uint16_t x, uint16_t y);
	void (*put_pixel)(uint16_t x, uint16_t y, rgb color);
	void (*set_mode)(uint8_t new_mode);
} DisplayDriver;
 *
 */

typedef struct MultiColor {
	union {
		uint8_t text_mode;
		rgb graphic_mode;
	} as;
} MultiColor;

#include "vga.h"

MultiColor stdcolor={.as.text_mode=(VGA_COLOR_LIGHT_GREY || (VGA_COLOR_BLACK << 4)), .as.graphic_mode={.r=0,.g=0,.b=0}};

#endif
