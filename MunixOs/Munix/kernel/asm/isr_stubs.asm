[bits 32]

%define KERNEL_CODE_SEG_SEL 0x08
%define KERNEL_DATA_SEG_SEL 0x10

%macro ISR_NOERRCODE 1
	global isr%1
	isr%1:
		cli
		push dword 0 ; código de error false
		push dword %1
		jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
	global isr%1
	isr%1:
		cli
		push dword %1
		jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0x00 ; Division By Zero
ISR_ERRCODE 0x08 ; Double Fault
ISR_ERRCODE 0x0D ; General Protection Fault

%assign i 1
%rep 7
	ISR_NOERRCODE i
%assign i i + 1
%endrep

ISR_NOERRCODE 0x09 ; Coprocessor Segment Overrun
ISR_NOERRCODE 0x0A ; Invalid TSS
ISR_NOERRCODE 0x0B ; Segment Not Present
ISR_NOERRCODE 0x0C ; Stack Segment Fault
ISR_ERRCODE 0x0E ; Page Fault
ISR_NOERRCODE 0x0F ; Reserved

%assign i 0x10
%rep 0x13-0x10+1
	ISR_NOERRCODE i
%assign i i + 1
%endrep

ISR_ERRCODE 0x11 ; Alignment check
ISR_NOERRCODE 0x12 ; Machine check
ISR_NOERRCODE 0x13 ; SIMD Floating-point Exception
ISR_NOERRCODE 0x14 ; Virtualization Exception
ISR_ERRCODE 0x15 ; Control Protection Exception

%assign i 0x16
%rep 0x1F - 0x16 + 1
	ISR_NOERRCODE I
%assign i i + 1
%endrep

isr_common_stub:
	cli

	push eax
	push ecx
	push edx
	push ebx
	push esp
	push ebp
	push esi
	push edi

	push ds

	mov ax, KERNEL_CODE_SEG_SEL
	mov 

