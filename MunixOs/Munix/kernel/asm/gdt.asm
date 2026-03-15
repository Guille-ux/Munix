[bits 32]

%define KERNEL_CS   0x08
%define KERNEL_DS   0x10
%define KERNEL_TSS  0x18

extern gdt_ptr

global gdt_flush
global reload_segment
global load_segment

segment dw 0

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

reload_segment:
	mov [segment], cs

	jmp [segment]:reload_selectors

	ret

reload_selectors:
	mov ax, ds
	
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret

load_segment:
	mov [segment], [esp + 4]

	jmp [segment]:load_selectors

	ret

load_selectors:
	mov ax, [esp + 8]

	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret
