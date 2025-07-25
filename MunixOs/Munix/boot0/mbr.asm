[BITS 16]
[ORG 0x7C00]

jmp _start
nop

boot_drive_port db 0x00

_start:
	mov [boot_drive_port], dl

	xor ax, ax ; para inicializarlo
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	call setup

	; código para cargar kernel
	mov ah, 0x02
	mov al, 255 
	mov ch, 0
	mov dh, 0
	mov cl, 0x02
	mov dl, [boot_drive_port]
	mov bx, 0x8000

	int 0x13

	jc _start

	; habilitar linea A20, sos, porque???
	; básicamente permitir más de un mega de ram
	in al, 0x92
	or al, 000000010b
	out 0x92, al

	; cargar gdt
	
	lgdt [gdtr]

	; activar modo protegido
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	; saltar al kernel
	jmp dword code_segment:0x8000
setup:
	mov ah, 0x00
	mov al, 0x03
	int 0x10
	ret
print: ; ok, aqui solo se pone caracter en bl, pues este controla el offset
	mov dx, ax
	mov ah, 0x0E
	mov al, bl
	int 0x10
	mov ax, dx
	ret
halt:
	jmp halt

gdt_start: ; TABLA GDT
	; Descriptor Nulo
	dd 0x0
	dd 0x0
code_segment:	; la info de como hay que hacer esto va sacada un poco de IA
		; aunque tengo excusa, por alguna razón no me iba la página de
		; osdev, F
	dw 0xFFFF ; 0-15 Limit
	dw 0x0000 ; base 0-15
	db 0x00	; base 16-23
	db 10011010b	; access byte
	db 11001111b	; granularity
	db 0x00	; base 24 - 31
data_segment:
	dw 0xFFFF ; 0-15 Limit
	dw 0x0000 ; base 0-15
	db 0x00	; base 16-23
	db 10011010b	; access byte
	db 11001111b	; granularity
	db 0x00	; base 24 - 31
gdt_end:

gdtr:
	dw gdt_end - gdt_start - 1
	dd gdt_start

times 440 - ($ - $$) db 0


; Código para las particiones etc, etc, etc

times 510 - ($ - $$) db 0

db 0x55
db 0xAA
