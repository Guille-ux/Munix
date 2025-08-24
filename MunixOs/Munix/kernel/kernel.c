#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/shell.h"
#include "init/init.h"
#include "fs/fs.h"
#include "disk/general.h"
#include "multiboot/multiboot.h"


void kernel_main(uint32_t magic, multiboot_info_t *mbi) {
	kernel_init();

	kprintf("~ MunixOs ~\n");

	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
