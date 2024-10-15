void shell() { //el bucle de la shell
	int seed = 0;
	char shellbuffer[BUFFER_SIZE];
	Pos posi;
	posi.pos = 0;
	posi.posc = 80;
	while (1) {
		putchar(posi.posc, '$');
		input(2, shellbuffer, posi.posc, 1);
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
		} if (strcmp(shellbuffer, "qwave\n")) {
			posi.posc += 80;
			for (int i = 0; i < 80; i++) {
				if (i % 2 == 0) {
					putcolor(0x21, posi.posc+i);
				}
			}
			posi.posc += 80;
		}
		buffclean(shellbuffer);
		if (posi.posc >= 80*23) {
			clear(0x0F);
			posi.posc = 0;
		}
	}
}

//cosas que tiene la shell
