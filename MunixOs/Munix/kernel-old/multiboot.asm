section .multiboot
	dd 0x1BADB002 ; Firma de Grub
	dd 0x00 ; Flags
	dd -(0x1BADB002 + 0x00) ; Checksum = complemento a 2 de Firma del grub y flags
