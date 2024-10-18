//display.h

#define CURSOR_COLOR 0x21
#define NORMAL_COLOR 0x0F
#define COLS 80
#define LINS 24


char cmat[LINS][COLS];

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
