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

//SINTAXIS
//
// ORD REG VALUE OUTREG
// MOV A 1 B the one says if you are going to put a number or the register but i can change this by T or F, R or N etc
// MOV A 0 9 numbers from 0 to 9
//
// MOV copy data
// ADD add numbers
// SUB substract
// MUL multiplicate
// DIV division
// JMP jump and the others JGT JNT JQT JLT......
// LOAD load anything from the ram
// STORE store anything in the ram
//
//END of sintaxis explaining

//ERROR CODES
//-1 ERROR IN OPCODE, BAD INSTRUCTION
//-2 ERROR IN THE OUTREGISTER, NOT DECLARED OR BAD DECLARED
//-3 ERROR IN VALUE, VALUE NOT DECLARED CORRECTLY
//-4 ERROR IN THE JUMP, BEFORE THE JUMP_TARGET YOU NEED TO ADD JMP 
//BAD SINTAX EXAMPLES AND GOOD SINTAX EXAMPLES
//
// MOV REG AX BX the order is bad, first instruction second out register third NUM or REG (in jumps is JMP) and in the end a number if you have choosen NUM or a register
// MOV AX REG BX good instruction
// JMP AX BX bad instruction you need to add JMP → JMP AX JMP BX
//
//JUMPS in jumps in the first reg you add the reg to compare in the second reg you put the TARGET
//I HAVE ADDED INTERRUPTS

#define INS_SIZE 5

typedef struct {
	char inst[INS_SIZE];
	int fpos;
} Ins;

int vasm_len(const char *str) {
	int len = 0;
	while (str[len] != '\0') {
		len++;
	}
	return len;
}

char vasm_strcmp(const char *str1, const char *str2, int len, int b) {
	for (int ch = 0; ch < len; ch++) {
		if (str1[ch+b] != str2[ch]) {
			return 0;
		}
	}
	return 1;
}



int asemble(const char *line) {
	int opcode = -1;
	int reg_idx = -1;
	int value = -1;
	int in = -1;
	if (vasm_strcmp(line, "MOV", 3, 0)) {
		opcode = 1;
	} else if (vasm_strcmp(line, "ADD", 3, 0)) {
		opcode = 2;
        } else if (vasm_strcmp(line, "SUB", 3, 0)) {
		opcode = 3;
        } else if (vasm_strcmp(line, "MUL", 3, 0)) {
		opcode = 4;
        } else if (vasm_strcmp(line, "DIV", 3, 0)) {
		opcode = 5;
        } else if (vasm_strcmp(line, "JNQ", 3, 0)) {
		opcode = 7;
		value = 4;
        } else if (vasm_strcmp(line, "LOA", 3, 0)) {
		opcode = 8;
        } else if (vasm_strcmp(line, "STR", 3, 0)) {
		opcode = 6;
        } else if (vasm_strcmp(line, "JLT", 3, 0)) {
		opcode = 7;
		value = 1;
        } else if (vasm_strcmp(line, "JGT", 3, 0)) {
		opcode = 7;
		value = 3;
        } else if (vasm_strcmp(line, "JQT", 3, 0)) {
		opcode = 7;
		value = 2;
        } else if (vasm_strcmp(line, "JMP", 3, 0)) {
		opcode = 7;
		value = 5;
	} else if (vasm_strcmp(line, "INT", 3, 0)) {
		opcode = 9;
		if (vasm_strcmp(line, "PUT", 3, 7)) {
			value = 1;
		} else {
			return -3;
		}
	}
	if (opcode < 0) { // MOV AX NUM 5
		return -1;
	}
        if (vasm_strcmp(line, "AX", 2, 4)) {
		reg_idx = 0;
        } else if (vasm_strcmp(line, "BX", 2, 4)) {
		reg_idx = 1;
        } else if (vasm_strcmp(line, "CX", 2, 4)) {
		reg_idx = 2;
        } else if (vasm_strcmp(line, "DX", 2, 4)) {
		reg_idx = 3;
        } else if (vasm_strcmp(line, "EX", 2, 4)) {
		reg_idx = 4;
        } else if (vasm_strcmp(line, "FX", 2, 4)) {
		reg_idx = 5;
        } else if (vasm_strcmp(line, "GX", 2, 4)) {
		reg_idx = 6;
        } else if (vasm_strcmp(line, "HX", 2, 4)) {
		reg_idx = 7;
        } else if (vasm_strcmp(line, "IX", 2, 4)) {
		reg_idx = 8;
	} if (reg_idx < 0) {
		return -2;
	} if (opcode == 7) {
	    if (vasm_strcmp(line, "JMP", 3, 7) == 0) {
	        return -4;
	    }
	} else if (value < 0) {
	        if (vasm_strcmp(line, "NUM", 3, 7)) {
			value = 0;
	        } else if (vasm_strcmp(line, "REG", 3, 7)) {
			value = 1;
		}
	} if (value < 0) {
		return -3;
	} if (opcode == 7 || value != 0) {
	        if (vasm_strcmp(line, "AX", 2, 11)) {
        	        in = 0;
	        } else if (vasm_strcmp(line, "BX", 2, 11)) {
        	        in = 1;
	        } else if (vasm_strcmp(line, "CX", 2, 11)) {
        	        in = 2;
	        } else if (vasm_strcmp(line, "DX", 2, 11)) {
        	        in = 3;
	        } else if (vasm_strcmp(line, "EX", 2, 11)) {
        	        in = 4;
	        } else if (vasm_strcmp(line, "FX", 2, 11)) {
	                in = 5;
	        } else if (vasm_strcmp(line, "GX", 2, 11)) {
        	        in = 6;
	        } else if (vasm_strcmp(line, "HX", 2, 11)) {
	                in = 7;
	        } else if (vasm_strcmp(line, "IX", 2, 11)) {
	                in = 8;
	        }
	} else if (value == 0){
		in = line[11] - '0';
	}
	int bytecode = 10000 + in * 1000 + value * 100 + reg_idx * 10 + opcode;
	return bytecode;
}


