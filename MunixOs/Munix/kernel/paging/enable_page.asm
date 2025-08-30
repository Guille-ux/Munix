global enable_paging32
global load_paging32

enable_paging32:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	ret

load_paging32:
	; mov eax, [esp + 4]
	pop eax
	mov cr3, eax
	push eax

	ret
