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

bitmap_t *bitmap=(bitmap_t*)0x00100000;
#define BITMAP_START ((size_t)bitmap + sizeof(bitmap_t))

pd32_entry_t *page_dir;

void munix_paging(multiboot_info_t *mbi) {
	mb_mmap_info_t mmap_info;
	multiboot1_build_mmap_info(&mmap_info, mbi);
	bitmap_init(bitmap, (void*)BITMAP_START, mbi->mem_upper, 4096);
	register_mmap_scanner(bitmap, multiboot_mmap_scanner);
	bitmap_scan_mmap(bitmap, &mmap_info);
	page_dir = create_page_directory32(bitmap);
	p32_flags_t flags = {.rw=1, .us=0, .pcd=0, .pwt=0};
	identity_mapping32(bitmap, page_dir, flags);
	load_paging32((uint32_t)page_dir);
	enable_paging32();
}

void kernel_main(uint32_t magic, multiboot_info_t *mbi) __attribute__((cdecl));

void kernel_main(uint32_t magic, multiboot_info_t *mbi) {
	kernel_init();
	munix_paging(mbi);
	kernel_init();
	kprintf("Magic Number: %p \n", magic);
	kprintf("Upper Memory: %lu\n", mbi->mem_upper);

	if (magic==MULTIBOOT_BOOT_MAGIC) {
			kprintf("Loaded by multiboot 1 compliant bootloader.\n");
	}

	kprintf("~ MunixOs ~\n");

	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
