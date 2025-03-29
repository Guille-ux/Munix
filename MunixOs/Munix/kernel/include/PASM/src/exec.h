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

#include "out-drivs.h"

#define RAM_LEN 512

typedef struct {
    int counter;
    int regs[9];
    int ram[RAM_LEN];
} VMachine;

void init_mach(VMachine *mach) {
    mach->counter = 0;
    for (int i = 0; i < 9; i++) {
        mach->regs[i] = 0;
    }
    for (int i = 0; i < 512; i++) {
        mach->ram[i] = 0;
    }
}

void exec(int ins, VMachine *mach) {
    mach->counter++;
    int opcode = ins % 10;
    ins /= 10;
    int reg_idx = ins % 10;
    int value = (ins / 10) % 10;
    int *outreg = &mach->regs[reg_idx];
    int in = (ins / 100) % 10;

    if (opcode == 1) {
        if (value == 0) {
            *outreg = in;
        } else {
            *outreg = mach->regs[in];
        }
    } else if (opcode == 2) {
        if (value == 0) {
            *outreg += in;
        } else {
            *outreg += mach->regs[in];
        }
    } else if (opcode == 3) {
        if (value == 0) {
            *outreg -= in;
        } else {
            *outreg -= mach->regs[in];
        }
    } else if (opcode == 4) {
        if (value == 0) {
            *outreg *= in;
        } else {
            *outreg *= mach->regs[in];
        }
    } else if (opcode == 5) {
        if (value == 0 && in != 0) {
            *outreg /= in;
        } else {
	    if (mach->regs[in] != 0) {
        	*outreg /= mach->regs[in];
	    }
        }
    } else if (opcode == 6) {
        mach->ram[mach->regs[reg_idx]] = mach->regs[in];
    } else if (opcode == 7) {
        int condition1 = value;
        int jump_target = mach->regs[in];

        int reg_val = mach->regs[reg_idx];
        int condition_met = 0;

        if (condition1 == 1 && reg_val < 0) {
            condition_met = 1;
        }
        if (condition1 == 2 && reg_val == 0) {
            condition_met = 1;
        }
        if (condition1 == 3 && reg_val > 0) {
            condition_met = 1;
        }
        if (condition1 == 4 && reg_val != 0) {
            condition_met = 1;
        } if (condition1 == 5) {
            condition_met = 1;
        }

        if (condition_met) {
            mach->counter = jump_target;
	}
    } else if (opcode == 8) {
        *outreg = mach->ram[mach->regs[in]];
    } else if (opcode == 9) {
        if (value == 1) {
            pch(mach->regs[reg_idx], mach->regs[in] / 80, mach->regs[in] % 80);
        }
    }
}

