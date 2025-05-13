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

#ifndef _DRAW_H
#define _DRAW_H

#include "pixels.h"
#include "types.h"
#include "io.h"

typedef struct {
    uint16_t x;
    uint16_t y;
} Vertex;

void draw_quad(Vertex a, Vertex b, uint8_t color) {
    for (uint16_t i =a.x; i<b.x; i++) {
        for (uint16_t z=a.y; i< b.y; i++) {
            write_pixel(i, z, color);
        }
    }
}

#endif