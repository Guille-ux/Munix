cd root/kernel
gcc -o funcs.o funcs.c
as -o kernel.o kernel.asm
ld -o START.ELF kernel.o funcs.o -Tlink.ld
