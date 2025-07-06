global idt_load32


idt_load32:
	mov eax, [esp+4]

	lidt [eax]

	ret
