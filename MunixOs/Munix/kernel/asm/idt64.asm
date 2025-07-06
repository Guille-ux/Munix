global idt_load64

idt_load64:
	lidt [rdi]
	ret
