cd Munix/kernel
i386-elf-gcc -m32 -c -o kernel.o kernel.c
i386-elf-ld -Tlinker.ld -o kernel.ELF kernel.o multiboot.o
cd ../..
grub-mkrescue -o munix.iso Munix
