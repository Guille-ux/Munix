; CAMBIOS DE CONTEXTO EN ASM
extern scheduler

global save_ctx
global restore_ctx

save_ctx:
	pushad ; cargamos todos los registros

	push esp

	call scheduler
	
	pop esp

	popad

	ret

restore_ctx:
	mov eax, [esp + 4]
	
	mov esp, eax
	
	sub esp, 32
	
	popad
	
	ret
