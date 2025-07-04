#include "../../headers/vga.h"
#include "../../include/display.h"
#include "../../include/vga.h"
#include "../../include/io.h"
#include "../../include/string.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

uint16_t height=25;
uint16_t width=80;
uint16_t global_x=0;
uint16_t global_y=0;
uint8_t current_mode=0x03;



uint8_t vga_seq_regs_mode_03[] = {
    0x03, // SR01: Clocking Mode Register
    0x01, // SR02: Map Mask Register
    0x0F, // SR03: Character Map Select
    0x00, // SR04: Memory Mode Register
    0x0E  // SR05: Address Mode
};
uint8_t VGA_SEQ_REG_COUNT_MODE_03 = sizeof(vga_seq_regs_mode_03);


uint8_t vga_crtc_regs_mode_03[] = {
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 
    0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00, 
    0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3, 
    0xFF                                            
};
uint8_t VGA_CRTC_REG_COUNT_MODE_03 = sizeof(vga_crtc_regs_mode_03);

uint8_t vga_gfx_regs_mode_03[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F,
    0xFF                                           
};
uint8_t VGA_GFX_REG_COUNT_MODE_03 = sizeof(vga_gfx_regs_mode_03);

uint8_t vga_attr_regs_mode_03[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x01, 0x00, 0x0F, 0x00, 0x00                   
};
uint8_t VGA_ATTR_REG_COUNT_MODE_03 = sizeof(vga_attr_regs_mode_03);


uint8_t vga_misc_out_reg_mode_03 = 0xE3;

void vga_setmode(uint8_t mode) {
	uint16_t i;
	uint8_t temp;
	switch (mode) {
		case (0x03): {
			temp = inb(VGA_MISC_READ); // read the current state
			outb(VGA_MISC_WRITE, temp & ~0xCF); // 0xCF = 11001111b, ~0xCF = 00110000
			
			outb(VGA_SEQ_INDEX_PORT, 0x00); // select a register from sequencer
			outb(VGA_SEQ_READ_WRITE_INDEX, 0x01);



			outb(VGA_MISC_WRITE, vga_misc_out_reg_mode_03); // for mode 0x03
			for (i=0;i<VGA_SEQ_REG_COUNT_MODE_03;i++) {
				outb(VGA_SEQ_INDEX_PORT, i+1);
				outb(VGA_SEQ_READ_WRITE_INDEX, vga_seq_regs_mode_03[i]);
			}
			
			outb(VGA_SEQ_INDEX_PORT, 0x00);
			outb(VGA_SEQ_READ_WRITE_INDEX, 0x03);


			outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_INDEX_PORT, 0x11);
			temp = inb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_READ_WRITE);
			outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_READ_WRITE, temp & 0x7F);

			for (i=0;i<VGA_CRTC_REG_COUNT_MODE_03;i++) {
				outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_INDEX_PORT, i);
				outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_READ_WRITE, vga_crtc_regs_mode_03[i]);
			}

			for (i=0;i<VGA_GFX_REG_COUNT_MODE_03;i++) {
				outb(VGA_GCR_INDEX_PORT, i);
				outb(VGA_GCR_READ_WRITE_INDEX, vga_gfx_regs_mode_03[i]);
			}

			inb(VGA_ACR_RESET);

			for (i=0;i<VGA_ATTR_REG_COUNT_MODE_03;i++) {
				outb(VGA_ACR_INDEX_WRITE_PORT, i);
				outb(VGA_ACR_INDEX_WRITE_PORT, vga_attr_regs_mode_03[i]);
				inb(VGA_ACR_RESET);
			}

			outb(VGA_ACR_INDEX_WRITE_PORT, 0x20); // enable video, idk
			outb(VGA_MISC_WRITE, vga_misc_out_reg_mode_03);

			     }
		default: return;
	}
}

void vga_clear(MultiColor color) {
	if (current_mode==0x03) {
		char chr=' ';
		for (uint16_t x=0;x<width;x++) {
			for (uint16_t y=0;y<height;y++) {
				vga_putchar(x, y, chr, color.as.text_mode);
			}
		}
	} else {
		// for the future
	}
}

void vga_putchar(uint16_t x, uint16_t y, char c, uint8_t attr) {
	if (x > width || y > height) return;
	vga_char *vga_ptr = (vga_char*)0xB8000 + x + y*width;

	vga_char chr;
	chr.character = c;
	chr.attr = attr;
	*vga_ptr = chr;
}

void vga_print_string(uint16_t x, uint16_t y, const char *str, uint8_t attr) {
	global_x = x;
	global_y = y;

	for (size_t i=0;i<strlen(str);i++) {
		char c=str[i];
		if (c=='\n') {
			global_x=0;
			global_y++;
		} else if (c=='\r') {
			global_x=0;
		} else {
			vga_putchar(global_x, global_y, c, attr);
			global_x++;
		}

		if (global_x>=width) {
			global_x=0;
			global_y++;
		}
		if (global_y>=height) {
			vga_scroll();
			global_y=height-1;
			global_x=0;
		}
	}
	vga_setcur(global_x, global_y);
}

void vga_setcur(uint16_t x, uint16_t y) {
	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;

	uint16_t pos = x + y*width;

	outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_INDEX_PORT, 0x0E); // read headers/vga.h
	outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_READ_WRITE, (uint8_t)((pos >> 8) & 0xFF)); // for little endian
	outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_INDEX_PORT, 0x0F); // read headers/vga.h
	outb(VGA_CRTC_INDEX_BASE_A + VGA_CRTC_READ_WRITE, (uint8_t)(pos & 0xFF));
}

void vga_putpix(uint16_t x, uint16_t y, rgb color) {
	// anything actually
}

void vga_scroll() {
	vga_char vacio = {' ', stdcolor.as.text_mode};
	memcpy((void *)VGA_TEXT_MODE_BEGIN, (const void *)(VGA_TEXT_MODE_BEGIN + width), sizeof(vga_char)*width*(height-1));
	vga_fill_line(height-1, vacio);

	global_x = 0;
	global_y = height-1;
	vga_setcur(global_x, global_y);
}

void vga_init(uint8_t mode) {
	vga_setmode(mode);	// actualmente solo necesito esto ¿no?
}

void vga_fill_line(uint16_t line, vga_char character) {
	if (current_mode==0x03) {
		for (uint16_t i=0;i<width;i++) {
			vga_putchar(i, line, character.character, character.attr);
		}
	} else {
		// for the future
	}
}

