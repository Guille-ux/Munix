cd root/kernel
gcc -o nexus.o nexus.c
as -o kernel.o kernel.asm
ld -o START.ELF kernel.o nexus.o -Tlink.ld
cd ../..
grub-mkrescue -o nexus.iso root/
