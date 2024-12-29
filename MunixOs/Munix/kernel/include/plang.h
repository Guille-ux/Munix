typedef struct {
    int counter;
    int regs[9];
    int ram[512];
} VMachine;

VMachine mach;

void exec(int ins) {
    int opcode = ins % 10;
    ins /= 10;
    int reg_idx = ins % 10;
    int value = (ins / 10) % 10;
    int *outreg = &mach.regs[reg_idx];

    if (opcode == 1) {
        *outreg = value;
    } else if (opcode == 2) {
        *outreg += value;
    } else if (opcode == 3) {
        *outreg -= value;
    } else if (opcode == 4) {
        *outreg *= value;
    } else if (opcode == 5) {
        if (value != 0) {
            *outreg /= value;
        }
    } else if (opcode == 6) {
        mach.ram[reg_idx] = mach.regs[value];
    } else if (opcode == 7) {
        int condition1 = ins % 10;
        ins /= 10;
        int condition2 = ins % 10;
        int jump_target = (ins / 10) % 10;

        int reg_val = mach.regs[reg_idx];
        int condition_met = 0;

        if (condition1 == 1 && reg_val < 0) {
            condition_met = 1;
        }
        if (condition2 == 1 && reg_val == 0) {
            condition_met = 1;
        }
        if (condition1 == 2 && reg_val > 0) {
            condition_met = 1;
        }
        if (condition2 == 2 && reg_val != 0) {
            condition_met = 1;
        }

        if (condition_met) {
            mach.counter = jump_target;
        }
    }
}
