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

#ifndef VRAM_H
#define VRAM_H

#define RAM_LEN 1000 // changing this 134217728 for this 1'000

#include "libs/lib.h"

typedef struct {
	char name[7];
	char data;
	char free;
} Var;

Var virtual_ram[RAM_LEN];

void ram_init() {
	for (int i = 0; i < RAM_LEN; i++) {
		virtual_ram[i].free = 1;
	}
}

int search_var(const char *name) { // ALL VARS NEED TO FINNISH IN FIN_SYMBOL AT LIBS.H
	for (int i = 0; i < RAM_LEN; i++) {
		if (vram_strcmp(virtual_ram[i].name, name)) {
			if (virtual_ram[i].free==0) {
				return i;
			}
		}
	}
	return -1;
}

int new_var(const char *name, char data) {
	int res = search_var(name);
	if (res > 0) {
		return 0;
	}
	for (int i = 0; i < RAM_LEN; i++) {
		if (virtual_ram[i].free) {
			virtual_ram[i].free = 0;
			for (int f = 0; f < 7; f++) {
				virtual_ram[i].name[f] = name[f];
			}
			virtual_ram[i].data = data;
			return 1;
		}
	}
}
void delete_var(const char *name) {
	int pos = search_var(name);
	if (pos >= 0) {
		virtual_ram[pos].free = 1;
	}
}

void write_var(const char *name, char data) {
	int pos = search_var(name);
	if (pos >= 0) {
		virtual_ram[pos].data = data;
	}
}

char read_var(const char *name) {
	int pos = search_var(name);
	if (pos > 0) {
		return virtual_ram[pos].data;
	}
	return -1;
}
#endif
