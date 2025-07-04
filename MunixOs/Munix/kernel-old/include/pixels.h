/* This program is free software: you can redistribute it and/or modify
/* it under the terms of the GNU General Public License as published by
/* the Free Software Foundation, either version 3 of the License, or
/* any later version.
/* This program is distributed in the hope that it will be useful,
/* but WITHOUT ANY WARRANTY; without even the implied warranty of
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/* GNU General Public License for more details.
/* You should have received a copy of the GNU General Public License
/* along with this program. If not, see <https://www.gnu.org/licenses/>.
/* Copyright (c) 2025 Guillermo Leira Temes
/* */

#ifndef _PIXELS_H
#define _PIXELS_H

#include "types.h"
#include "io.h"

#define VGA_MODE 0x12
#define DISPLAY_HEIGHT 480
#define DISPLAY_WIDTH 640
#define VGA_BUFFER_ADDRESS 0xA0000
#define BYTES_PER_LINE 80
#define BITS_PER_PIXEL 4 // 16 colors

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t color;
} Pixel; //this is only for the future

void write_pixel(uint16_t x, uint16_t y, uint8_t color) {
    if (x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) {
        return; // if not valid, don't do anything
    }
    uint32_t offset = y * BYTES_PER_LINE + x / (8 / BITS_PER_PIXEL); // calc's the offset
    uint8_t *vga_buffer = (uint8_t *)VGA_BUFFER_ADDRESS; // get's the buffer address

    uint8_t shift=(x % (8/BITS_PER_PIXEL)) * BITS_PER_PIXEL;
    uint8_t byte= (color << shift) | vga_buffer[offset];

    vga_buffer[offset]=byte;
}

#endif