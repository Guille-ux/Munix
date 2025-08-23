global enable_paging32

enable_paging32:
	pop cr3
	
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	ret
