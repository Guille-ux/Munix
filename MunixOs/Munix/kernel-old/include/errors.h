/* This program is free software: you can redistribute it and/or modify
/* it under the terms of the GNU General Public License as published by
/* the Free Software Foundation, either version 3 of the License, or
/* (at your option) any later version.
/* This program is distributed in the hope that it will be useful,
/* but WITHOUT ANY WARRANTY; without even the implied warranty of
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/* GNU General Public License for more details.
/* You should have received a copy of the GNU General Public License
/* along with this program. If not, see <https://www.gnu.org/licenses/>.
/* Copyright (c) 2025 Guillermo Leira Temes
/* */

#ifndef _KERROR_H
#define _KERROR_H

#include "display.h"
#include "stnio.h"
void raise_zero_error() {
	clear(0x4F);
	print("Zero Division Error", 20, 0);
}

void mem_error() {
	clear(0x4F);
	print("Not Enough Memory", 20, 0);
}

#endif
