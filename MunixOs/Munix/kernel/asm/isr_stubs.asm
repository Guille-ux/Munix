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


ISR_NOERRCODE 0 ; 0x00  Divide Error
ISR_NOERRCODE 1 ; 0x01  Debug Exception
ISR_NOERRCODE 2 ; 0x02  NMI Interrupt
ISR_NOERRCODE 3 ; 0x03  Breakpoint
ISR_NOERRCODE 4 ; 0x04  Overflow
ISR_NOERRCODE 5 ; 0x05  BOUND Range Exceeded
ISR_NOERRCODE 6 ; 0x06  Invalid Opcode
ISR_NOERRCODE 7 ; 0x07  Device Not Available (No Math Coprocessor)
ISR_ERRCODE   8 ; 0x08  Double Fault 
ISR_NOERRCODE 9 ; 0x09  Coprocessor Segment Overrun 
ISR_ERRCODE   10 ; 0x0A  Invalid TSS 
ISR_ERRCODE   11 ; 0x0B  Segment Not Present 
ISR_ERRCODE   12 ; 0x0C  Stack-Segment Fault 
ISR_ERRCODE   13 ; 0x0D  General Protection Fault 
ISR_ERRCODE   14 ; 0x0E  Page Fault 

ISR_NOERRCODE 15 ; 0x0F  Reserved 

ISR_NOERRCODE 16 ; 0x10  x87 FPU Floating-Point Error
ISR_ERRCODE   17 ; 0x11  Alignment Check 
ISR_NOERRCODE 18 ; 0x12  Machine Check
ISR_NOERRCODE 19 ; 0x13  SIMD Floating-Point Exception
ISR_NOERRCODE 20 ; 0x14  Virtualization Exception
ISR_ERRCODE   21 ; 0x15  Control Protection Exception 


%assign i  0x16
%rep 0x1F - 0x16 + 1 
    ISR_NOERRCODE i
%assign i  i + 1
%endrep



%assign i  0x20
%rep 0x2F - 0x20 + 1 
    ISR_NOERRCODE i
%assign i  i + 1
%endrep


%assign i  0x30
%rep 0xFF - 0x30 + 1
    ISR_NOERRCODE i
%assign i  i + 1
%endrep

extern isr_handler

isr_common_stub:
	cli

	pusha

	push ds
	push es
	push fs
	push gs

	mov ax, KERNEL_DATA_SEG_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push esp

	call isr_handler

	add esp, 4 ; descartar el valor de esp_dummy

	pop gs
	pop fs
	pop es
	pop ds

	popa

	add esp, 8

	sti ; rehabilitar las interrupciones


	iret ; volver desde donde se llamo la interrupt

