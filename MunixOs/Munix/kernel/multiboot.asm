section .multiboot
	dd 0x1BADB002 ; Firma de Grub
	dd 0x00 ; Flags
	dd -(0x1BADB002 + 0x00) ; Checksum = complemento a 2 de Firma del grub y flags

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
		resb 16384
	stack_top:
