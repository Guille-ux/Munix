#define CODE_LEN 4096 //number of instructions can handle a process
#define MAX_P 1024

typedef struct {
	char free; //is the task free or not
	uint16_t votes; // how many process need this process or how many blablabalablalabla
	uint16_t id; // Process ID
	uint16_t code[CODE_LEN]; // all the code this thing is going to run, code for plang.h
} process_t;

process_t ps[MAX_P];

uint16_t actual_id = 0;

void paste_code(uint16_t *source, uint16_t *destination, int length) {
    for (int i = 0; i < length; i++) {
        destination[i] = source[i];
    }
}

void attach_code(uint16_t id, uint16_t *code) {
	paste_code(code, ps[id].code, CODE_LEN);
}

void new_process(uint16_t votes) {
	int id= -1;
	for (unsigned int i=0; i<MAX_P; i++) {
		if (ps[i].free) {
			id = i;
			break;
		}
	}
	if (id<0) {
		mem_error();
	} else {
		ps[id].free=0;
		ps[id].votes=votes;
		ps[id].id=id;
		actual_id++;	
	}
}
void kill_process(uint16_t id) {
	ps[id].free=1;
}

void manage() {
	
}
