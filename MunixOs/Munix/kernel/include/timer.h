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

#define PIT_TIME 0x36
#define PIT_CHA0 0x40
#define PIT_READ_COM 0x00


volatile uint32_t seconds = 0;

void pit_time_init() {
	uint16_t divisor = PIT_FREQ;
	outb(PIT_2, PIT_TIME);
	outb(PIT_CHA0, divisor & 0xFF);
	outb(PIT_CHA0, (divisor >> 8) & 0xFF);
}
void rst_time() {
	seconds = 0;
}

uint16_t pit_read_counter() {
	uint8_t hi, lo;
	outb(PIT_CHA0, PIT_READ_COM);
	//read
	lo = inb(PIT_CHA0);
	hi = inb(PIT_CHA0);
	return (hi << 8) | lo;
}

