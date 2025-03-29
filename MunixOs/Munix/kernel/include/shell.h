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

#ifndef MSHELL_H
#define MSHELL_H

void shell() { //el bucle de la shell
	print_banner();
	save_mat();
	int seed = 0;
	char shellbuffer[BUFFER_SIZE];
	Pos posi;
	posi.pos = 0;
	posi.posc = 80;
	while (1) {
		putchar(posi.posc, '$');
		input(2, shellbuffer, posi.posc, 1, 1);
		if (ccmp(shellbuffer, "stalin", 6)) {
			posi.posc += 80;
			if (seed == 0) {
				seed = len(shellbuffer);
			}
			seed = small_random(seed);
			if (seed % 2 == 0) {
				print("Stalin Say's Yes", 16, posi.posc);
			} else {
				print("Stalin Say's No", 15, posi.posc);
			}
			posi.posc += 80;
		} else if (strcmp(shellbuffer, "error\n")) {
			raise_zero_error();
			save_mat();
		} else if (strcmp(shellbuffer, "clear\n")) {
			posi.posc = 0;
			clear(NORMAL_COLOR);
			load_mat();
		}
		buffclean(shellbuffer);
		if (posi.posc >= 80*23) {
			clear(0x0F);
			posi.posc = 0;
			posi.pos = 3;
		}
	}
}

//cosas que tiene la shell
#endif
