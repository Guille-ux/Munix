OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
	. = 1M;
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
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
	.kernel_stack BLOCK(10M) : ALIGN(4K) {
		stack_top = .;
		. = . + 10M;
		stack_bottom = .;
	}
}
