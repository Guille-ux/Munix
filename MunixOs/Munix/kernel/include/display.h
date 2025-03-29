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

//display.h
#ifndef _DISPLAY_H
#define _DISPLAY_H

#define CURSOR_COLOR 0x21
#define NORMAL_COLOR 0x0F
#define COLS 80
#define LINS 24


char cmat[LINS][COLS];

void print_banner() {
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < COLS; x++) {
			if (y == 0) {
				putcolor(RED << 4, COLS*y+x);
			} else if (y == 1) {
				putcolor(YELLOW << 4, COLS*y+x);
			} else if (y == 2) {
				putcolor(MAGENTA << 4, COLS*y+x);
			}
		}
	}
}

void init_mat() {
	clear(NORMAL_COLOR);
	save_mat();
}

void drawc(int pos, int colmat) {
	char *cpt = (char *)0xB8000;
	fill(NORMAL_COLOR);
	if (colmat == 1) {
		load_mat();
	}
	cpt[(pos*2)+1] = CURSOR_COLOR;
}
void fill(char color) {
	for (int i = 0; i < COLS*LINS; i++) {
		char *cpt = (char *)0xB8000;
		cpt[i*2+1] = color;
	}
}
void clear(char color) {
	for (int i = 0; i < COLS*LINS; i++) {
		char *cpt = (char *)0xB8000;
		cpt[i*2+1] = color;
		putchar(i, ' ');
	}
}
void putcolor(char color, int pos) {
	char *cpt = (char *)0xB8000;
	cpt[pos*2+1] = color;
}
void save_mat() {
	for (int x = 0; x < LINS; x++) {
		for (int y = 0; y < COLS; y++) {
			char *cpt = (char *)0xB8000;
			cmat[x][y] = cpt[(x*COLS+y)*2+1];
		}
	}
}
void load_mat() {
        for (int x = 0; x < LINS; x++) {
                for (int y = 0; y < COLS; y++) {
                        char *cpt = (char *)0xB8000;
                        cpt[(y+x*COLS)*2+1] = cmat[x][y];
                }
        }

}

#endif
