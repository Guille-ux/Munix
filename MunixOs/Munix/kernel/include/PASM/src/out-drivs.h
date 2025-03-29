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

#ifdef MUNIX_KERNEL

void pch(char ch, int x, int y) {
  putchar(ch, x+y*80);
}

#endif

#ifndef MUNIX_KERNEL
#include <stdio.h>

void gotoxy(int x,int y)    
{
    printf("%c[%d;%df",0x1B,y,x);
}
void pch(char ch, int x, int y) {
  gotoxy(x, y);
  putchar(ch);
}

#endif
