#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/shell.h"
#include "init/init.h"
#include "fs/fs.h"
#include "disk/general.h"
#include "multiboot/multiboot.h"


#include "memory/bitmap.h"
#include "paging/paging32.h"
#include "multiboot/multiboot_mmap.h"

/*
 * Temporalmente tendre la paginación aqui
 */

extern char _bitmap_data;
extern char _bitmap_start;
extern char _kernel_start;
extern char _kernel_end;

bitmap_t *bitmap=(bitmap_t*)(&_bitmap_data);

pd32_entry_t *page_dir;
p_block_t block __attribute__((aligned(4096), section(".pgtable")));

void munix_paging() {
	kprintf("paging begins...\n");	
	page_dir = (pd32_entry_t*)&block;
	p32_flags_t flags = {.rw=1, .us=1, .pcd=0, .pwt=0};
	identity_mapping32(bitmap, page_dir, flags);
	load_paging32((uint32_t)page_dir);
	enable_paging32();
}

void kernel_main(uint32_t magic, multiboot_info_t *mbi) __attribute__((cdecl));

void kernel_main(uint32_t magic, multiboot_info_t *mbi) {
	kernel_init(mbi); // para poder debuggear con kprintf
	munix_paging();
	kprintf("Magic Number: %p \n", magic);
	kprintf("Upper Memory: %lu\n", mbi->mem_upper);
	//kprintf("Page Dir Addr: %p\n", page_dir);
	//kprintf("PGTABLE SECTION START: %p\n", (uint32_t)&_paging_section_start);

	if (magic==MULTIBOOT_BOOT_MAGIC) {
			kprintf("Loaded by multiboot 1 compliant bootloader.\n");
	}

	kprintf("~ MunixOs ~\n");

	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
