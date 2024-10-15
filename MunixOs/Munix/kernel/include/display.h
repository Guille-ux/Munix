//display.h

#define CURSOR_COLOR 0x21
#define NORMAL_COLOR 0x0F
#define COLS 80
#define LINS 24

void drawc(int pos) {
	char *cpt = (char *)0xB8000;
	fill(NORMAL_COLOR);
	cpt[pos*2+1] = CURSOR_COLOR;
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
