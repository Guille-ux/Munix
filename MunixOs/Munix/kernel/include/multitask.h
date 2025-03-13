#define CODE_LEN 4096 //number of instructions can handle a process
#define MAX_P 1024
#define CYCLES 1024

typedef struct {
	char free; //is the task free or not
	uint16_t votes; // how many process need this process or how many blablabalablalabla
	uint16_t id; // Process ID
	uint16_t code[CODE_LEN]; // all the code this thing is going to run, code for plang.h
	VMachine mach;
	uint16_t cycles;
} process_t;

process_t ps[MAX_P];

uint16_t actual_id = 0; // mide la cantidad de procesos actualmente

void paste_code(uint16_t *source, uint16_t *destination, int length) {
    for (int i = 0; i < length; i++) {
        destination[i] = source[i];
    }
}

void attach_code(uint16_t id, uint16_t *code) {
	paste_code(code, ps[id].code, CODE_LEN);
}

int new_process(uint16_t votes) {
	int id= -1;
	for (unsigned int i=0; i<MAX_P; i++) {
		if (ps[i].free) {
			id = i;
			break;
		}
	}
	if (id<0) {
		mem_error();
		return 0;
	} else {
		ps[id].free=0;
		ps[id].votes=votes;
		ps[id].id=id;
		init_mach(&ps[id].mach);
		actual_id++;
	}
	return id;
}
void kill_process(uint16_t id) {
	ps[id].free=1;
	ps[id].votes=0
	actual_id--;
}

void calc_priors(unsigned int cycles, unsigned int total_votes) {
	for (int i=0; i < MAX_P; i++) {
		ps[i].cycles = (ps[i].votes / total_votes) * cycles;
	}
}
void manage(unsigned int n_cycles) {
	unsigned int total_votes;
	for (int i=0; i < MAX_P; i++) {
		total_votes += ps[i].votes;
	}
	calc_priors(CYCLES, total_votes);
	for (unsigned int i=0; i<n_cycles;i++) {
		for (int i=0; i<MAX_P;i++) {
			if (ps[i].free==0) {
				for (int j=0; j<ps[i].cycles; j++) {
					if (ps[i].mach.counter >= CODE_LEN) {
						kill_process(i);
						break;
					}
					else if (ps[i].code[ps[i].mach.counter]==0) {
						kill_process(i);
						break;
					}
					else {
						exec(ps[i].code[ps[i].mach.counter], ps[i].mach);
					}
				}
			}
		}
	}
}
