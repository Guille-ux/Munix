OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
	. = 0x00A00000;
	_kernel_start = .;	
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}

	.pgtable BLOCK(4K) : ALIGN(4K) {
		_paging_section_start = .;
		*(.pgtable)
		_paging_section_end = .;
	}
	.bitmap BLOCK(4K) : ALIGN(4K) {
		_bitmap_data = .;
		. = . + 4K;
		_bitmap_start = .;
		. = . + 128K;
		_bitmap_end = .;
	}
	.info BLOCK(4K) : ALIGN(4K) {
		_kernel_data_start = .;
		. = . + 64K;
		_kernel_data_end = .;
	}

	.rodata BLOCK(32K) : ALIGN(4K) {
		*(.rodata)
	}
	.data BLOCK(16K) : ALIGN(4K) {
		*(.data)
	}
	.bss BLOCK(5M) : ALIGN(4K) {
		*(.bss)
		*(COMMON)
	}
	_kernel_end = .;
}
