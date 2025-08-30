%define MAGIC 0x1BADB002
%define MEM_INFO 1
%define FLAGS MEM_INFO
%define CHECKSUM - (MAGIC + FLAGS)

section .multiboot
	dd MAGIC ; Firma de Grub
	dd FLAGS ; Flags
	dd CHECKSUM ; Checksum = complemento a 2 de Firma del grub y flags

section .text
	bits 32

	global _start

	extern kernel_main

	_start:
		mov esp, stack_top

		push ebx
		push eax

		call kernel_main
	.hang:
		cli
		hlt
		jmp .hang
section .bss
	kernel_stack:
		resb 1024*32
	stack_top:
