OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_entry)
SECTIONS
{
	. = 1M;
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}
	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}
	.bss BLOCK(4K) : ALIGN(4K) {
		*(.bss)
	}
}
