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

#include "pasm.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 20
#define MAX_INS 65536

VMachine mach;

void compile_run(const char *file) {
  FILE *f = fopen(file, "r");
  char buffer[MAX_LEN];
  int instructions[MAX_INS];
  int time = 0;
  while (fgets(buffer, MAX_LEN, f)) {
    strtok(buffer, '\n');
    instructions[time] = asemble(buffer);
    if (asemble(buffer) == -1) {
      break;
    }
    time++;
  }
  init_mach(&mach);
  while (mach.counter < time) {
    exec(instructions[mach.counter], &mach);
    mach.counter++;
  }
}
