#define CODE_LENGTH 1024 //number of instructions can handle a process

typedef struct {
	int free; //is the task free or not
	int votes; // how many process need this process or how many blablabalablalabla
	int length; //to help to the os to know how many time need this process
	uint16_t code[CODE_LENGTH]; // all the code this thing is going to run, code for plang.h
}

void paste_code(uint16_t *source, uint16_t *destination, int length) {
    for (int i = 0; i < length; i++) {
        destination[i] = source[i];
    }
}
