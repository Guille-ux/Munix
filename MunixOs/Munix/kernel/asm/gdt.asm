[bits 32]

%define KERNEL_CS   0x08
%define KERNEL_DS   0x10

extern gdt_ptr

global gdt_flush

gdt_flush:
	lgdt [gdt_ptr]

	jmp KERNEL_CS:flush_cs

flush_cs:
	mov ax, KERNEL_DS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret
