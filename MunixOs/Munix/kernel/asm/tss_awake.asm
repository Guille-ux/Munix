; aqui levantaremos al task state segment para q funcione
%define KERNEL_TSS 0x18

global awake_tss

awake_tss:
	mov ax, KERNEL_TSS
	ltr ax

	ret
