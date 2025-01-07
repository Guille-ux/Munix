#define RAM_LEN 134217728

typedef struct {
	const char name[6];
	char data;
	char free = 1;
} Var;

Var virtual_ram[RAM_LEN]; 

void ram_init() {
	for (int i = 0; i < RAM_LEN; i++) {
		virtual_ram[i].free = 1;
	}
}

int new_var(const char *name, char data) {
	int res = search_var();
	if (res > 0) {
		return 0;
	}
	for (int i = 0; i < RAM_LEN; i++) {
		if (virtual_ram[i].free) {
			return 1;
			virtual_ram[i].free = 0;
			for (int f = 0; f < 6; f++) {
				virtual_ram[i].name[f] = name[f];
			}
		}
	}
}
int search_var(const char *name) {
	for (int i = 0; i < RAM_LEN; i++) {
		if (strcmp(virtual_ram[i].name, name)) {
			return i;
		}
	}
	return -1;
}
