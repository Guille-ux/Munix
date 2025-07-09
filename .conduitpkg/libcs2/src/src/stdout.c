#include "../include/stdio.h"
#include "../include/vga.h"
#include "../include/stdbuff.h"
#include "../include/string.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


stdout stdout_interface={.active=false, .default_color={.as.text_mode=0x0F, .as.graphic_mode={0x0F, 0x0F, 0x0F}}};

void kclear() {
	stdout_interface.clear(stdcolor);
}

void stdout_init_vga() {
	stdout_interface.get_max_x=vga_get_max_x;
	stdout_interface.get_max_y=vga_get_max_y;
	stdout_interface.get_cur_x=vga_get_cur_x;
	stdout_interface.get_cur_y=vga_get_cur_y;
	stdout_interface.print_string=vga_print_string,
	stdout_interface.putchar=vga_putchar;
	stdout_interface.setcur=vga_setcur;
	stdout_interface.clear=vga_clear;
	stdout_interface.fill_line=vga_fill_line;
	stdout_interface.scroll=vga_scroll;
	stdout_interface.default_color.as.text_mode=0x0F;
}

void kchcolor(MultiColor new_color) {
	stdout_interface.default_color.as.text_mode=new_color.as.text_mode;
	stdout_interface.default_color.as.graphic_mode.r=new_color.as.graphic_mode.r;
	stdout_interface.default_color.as.graphic_mode.g=new_color.as.graphic_mode.g;
	stdout_interface.default_color.as.graphic_mode.b=new_color.as.graphic_mode.b;
}

void disable_stdout() {
	stdout_interface.active=false;
}

void enable_stdout() {
	stdout_interface.active=true;
}

// Variables de STDBUFF

MultiColor *color_buffer;
char *buffer;
size_t buff_height;
size_t buff_width;
size_t buff_cur_x=0;
size_t buff_cur_y=0;

// implementación de las funciones para el buffer (STDBUFF)


void buff_init(char *buff, MultiColor *color_buff, size_t height, size_t width) {
	buff_height=height;
	buff_width=width;
	buffer=buff;
	color_buffer=color_buff;
}

void buff_setcur(size_t x, size_t y) {
	buff_cur_x = x;
	buff_cur_y = y;
}

void buff_print_string(size_t x, size_t y, const char *str, MultiColor color) {
	buff_cur_x=x;
	buff_cur_y=y;

	for (size_t i=0;i<strlen(str);i++) {
		char c=str[i];
		if (c=='\n') {
			buff_cur_x=0;
			buff_cur_y++;
		} else if (c=='\r') {
			buff_cur_x=0;
		} else {
			buff_putchar(buff_cur_x, buff_cur_y, c, color);
			buff_cur_x++;
		}

		if (buff_cur_x>=buff_width) {
			buff_cur_x=0;
			buff_cur_y++;
		}
		if (buff_cur_y>=buff_height) {
			buff_scroll();
			buff_cur_y=buff_height-1;
			buff_cur_x=0;
		}
	}
}

void buff_putchar(size_t x, size_t y, char c, MultiColor color) {
	if (x > buff_width || y > buff_height) return;
	buffer[x+y*buff_width]=c;
	color_buffer[x+y*buff_width]=color;
}

void buff_clear(MultiColor color) {
	for (size_t x=0;x<buff_width;x++) {
		for (size_t y=0;y<buff_height;y++) {
			buff_putchar(x, y, ' ', color);
		}
	}
	buff_setcur(0, 0);
}

void buff_scroll() {
	memcpy((void *)buffer, (const void *)(buffer + buff_width), buff_width*(buff_height-1));
	memcpy((void*)color_buffer, (const void*)(color_buffer + buff_width), sizeof(MultiColor)*buff_width*(buff_height-1));
	for (size_t i=0;i<buff_width;i++) {
		buff_putchar(i, buff_height-1, ' ', stdcolor);
	}
	buff_cur_x = 0;
	buff_cur_y = buff_height-1;
	buff_setcur(buff_cur_x, buff_cur_y);
}

void buff_fill_line(size_t line, vga_char character) {
	if (line>buff_height) return;
	MultiColor color={.as.text_mode=character.attr, .as.graphic_mode={0, 0, 0}};
	for (size_t i=0;i<buff_width;i++) {
		buff_putchar(i, line, character.character, color);
	}
}

size_t buff_get_max_x() {
	return buff_width;
}

size_t buff_get_max_y() {
	return buff_height;
}

size_t buff_get_cur_x() {
	return buff_cur_x;
}

size_t buff_get_cur_y() {
	return buff_cur_y;
}

void stdout_init_buffer() {
	stdout_interface.get_max_x=buff_get_max_x;
	stdout_interface.get_max_y=buff_get_max_y;
	stdout_interface.get_cur_x=buff_get_cur_x;
	stdout_interface.get_cur_y=buff_get_cur_y;
	stdout_interface.print_string=buff_print_string,
	stdout_interface.putchar=buff_putchar;
	stdout_interface.setcur=buff_setcur;
	stdout_interface.clear=buff_clear;
	stdout_interface.fill_line=buff_fill_line;
	stdout_interface.scroll=buff_scroll;
}


