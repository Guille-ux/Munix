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

//stnio.h
#ifndef _STNIO_H
#define _STNIO_H

#include "keyboard.h"
#include "display.h"

void putchar(int pos, unsigned char ch) {
    char *cpt = (char *)0xB8000;
    cpt += pos * 2;
    *cpt = ch;
}
char getchar() {
    unsigned char key;
    asm volatile("inb $0x60, %0" : "=a"(key));
    return key;
}

void print(const char *str, int lenght, int pos) {
    for (int i = 0; i < lenght; i++) {
        putchar(pos + i, *str);
        str++;
    }
}

char rchar(unsigned char scancode) {
    return en_US[scancode];
}

char getch() {
    unsigned char key;

    // Leer el puerto de estado 0x64 para verificar si hay datos disponibles en 0x60
    while (1) {
        unsigned char status;
        asm volatile("inb $0x64, %0" : "=a"(status));

        if (status & 0x01) { // Si el bit 0 está encendido, hay datos disponibles
            asm volatile("inb $0x60, %0" : "=a"(key));

            // Ignorar el código si corresponde a la liberación de una tecla
            if (!(key & 0x80)) {
                return key;
            }
        }
    }
}

char bioschar() {
	char tecla;
	asm volatile ("int $0x16" : "=a"(tecla) : "a"(0x0000));
	return tecla;
}
char bioscharn() {
	char tecla;
	asm volatile("int $0x16" : "=a"(tecla) : "a"(0x0100));
	return tecla;
}
void buffer_handler(int buffpos, char scancode, char *buffer) {
	if (buffpos == BUFFER_SIZE) {
		for (int i = 0; i < BUFFER_SIZE; i++) {
			buffer[i] = 0;
		}
	} else if (scancode == 0x0E) {
		buffer[buffpos+1] = 0;
	} else {
		buffer[buffpos] = en_US[scancode];
	}
}
Pos key_handler(Pos pos, char scancode, int min, char *buffer, int all) {
	if (pos.pos+pos.posc-min == BUFFER_SIZE && scancode != 0x0E) {
		return pos;
	} else {
	        if (scancode == 0x0E) {
		    if (pos.pos+pos.posc-min != 0) {
			if (all == 1) {
				if (pos.pos-min > 0) {
					pos.pos--;
					putchar(pos.pos+pos.posc, ' ');
				} else {
					pos.pos++;
				}
			} else {
		        	pos.pos--;
		        	putchar(pos.pos+pos.posc, ' ');
			}
                    }
	        } else if (scancode == 0x1C) {
		    pos.posc += 80-((pos.pos+pos.posc) % 80);
	        } else {
		    putchar(pos.pos+pos.posc, en_US[scancode]);
		    pos.pos++;
		    buffer[pos.pos] = en_US[scancode];
		    return pos;
	        }
		buffer_handler(pos.pos+pos.posc-min, scancode, buffer);
	}
	return pos;
}


int input(int len, char *buffer, int line, int all, int ac) {
	Pos posi;
	posi.pos = len;
	posi.posc = line;
	int now = 0;
	unsigned char scancode = 0;
	while (scancode != 0x1C) {
		scancode = getch();
		posi = key_handler(posi, scancode, len, buffer, all);
		drawc(posi.pos+posi.posc, ac);
		buffer[now] = en_US[scancode];
		if (scancode == 0x0E) {
			if (now > 0) {
				buffer[now] = 0;
				now--;
			}
		} else if (BUFFER_SIZE == now) {
			now--;
		} else {
			buffer[now] = en_US[scancode];
			now++;
		}
	}
}
void buffclean(char *buffer) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
}

#endif
