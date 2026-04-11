[bits 32]

%define KERNEL_CS   0x08
%define KERNEL_DS   0x10
%define KERNEL_TSS  0x18

extern gdt_ptr

global gdt_flush
global reload_segment
global load_segment

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
	push load_selectors
	push cs

	retf

reload_selectors:
	mov ax, ds
	
	mov ds, ax
	mov es, ax
	mov fs, 48
	mov gs, ax
	mov ss, ax

	ret

load_segment:
	push [esp + 8]
	push load_selectors
	push [esp + 12]

	retf

load_selectors:
	pop ax

	mov ds, ax
	mov es, ax
	mov fs, 48
	mov gs, ax
	mov ss, ax

	ret
