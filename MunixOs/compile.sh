#!/bin/bash

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
# Copyright (c) 2025 Guillermo Leira Temes
# 
cd Munix/kernel
nasm -f elf32  -o idt_load.o asm/idt32.asm
# para idt 64 nasm -f elf64 -o idt_load.o asm/idt64.asm
nasm -f elf32 -o isr_stubs.o asm/isr_stubs.asm
nasm -f elf32 -o gdt_load.o asm/gdt.asm
nasm -f elf32 -o multiboot.o multiboot.asm
cd mbash
gcc -fno-stack-protector -m32 -c *.c
cd ..
cd minim
gcc -fno-stack-protector -m32 -c *.c
cd ..
cd disk
gcc -fno-stack-protector -m32 -c *c
cd ..
gcc -fno-stack-protector -m32 -c -o buddy.o src/buddy_alloc.c
gcc -fno-stack-protector -m32 -c -o shell.o src/shell.c
gcc -fno-stack-protector -m32 -c -o timer.o src/timer.c
gcc -fno-stack-protector -m32 -c -o pic.o src/pic.c
gcc -fno-stack-protector -m32 -c -o isr.o src/isr.c
gcc -fno-stack-protector -m32 -c -o gdt.o src/gdt.c
gcc -fno-stack-protector -m32 -c -o idt.o src/idt.c
gcc -fno-stack-protector -m32 -c -o kernel.o kernel.c
gcc -fno-stack-protector -m32 -c -o memory.o src/memory.c
gcc -fno-stack-protector -m32 -c -o ksysarena.o src/sysarena.c
i386-elf-ld -Tlinker.ld -o kernel.ELF disk/*.o buddy.o minim/*.o mbash/*.o shell.o timer.o ksysarena.o pic.o memory.o isr.o gdt_load.o gdt.o isr_stubs.o idt_load.o idt.o kernel.o multiboot.o -L../../libs -lcs2 # -lzynk
cd ../..
grub-mkrescue -o munix.iso Munix
