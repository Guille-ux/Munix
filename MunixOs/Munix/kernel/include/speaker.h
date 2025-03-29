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

#define PIT_FREQ 1193182
#define PIT_2 0x43    // Puerto de datos del PIT 2
#define PIT_2_W 0xB6  // Comando para configurar el PIT 2
#define SPEAKER 0x61

void speaker_on() {
	char status = inb(SPEAKER);
	outb(SPEAKER, status | 0x03);
}
void speaker_off() {
	char status = inb(SPEAKER);
	outb(SPEAKER, status & 0xFC);
}
void play_tone(int freq) {
	uint16_t divisor = PIT_FREQ / freq; // freq in khz
	outb(PIT_2, PIT_2_W);
	outb(PIT_2-1, divisor & 0xFF);
	outb(PIT_2-1, (divisor >> 8) & 0xFF);
}
