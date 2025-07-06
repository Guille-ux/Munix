org 0x7C00
bits 16

_start:
	jmp $


times 510 - ($ - $$) db 0
dw 0xAA55
