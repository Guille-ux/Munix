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

	//__asm__ volatile("cli");

	/*
	 * Nota, el código siguiente fue usado durante pruebas.
	 */ 

	// MBRformatMFS(&system_disks[0], 0, 4, uint64_2_lba(1), 2047);

	/*explorer_t explorer;
	while (1) {
		if (mfs_init_explorer(&kpartition_manager.partitions[0], &explorer, 0, 0)!=NULL) break;
	}

	mfs_superblock_t *block = ((mfs_meta_t*)explorer.meta)->superblock;
	kprintf("Root Block: %d\n", block->RootBlock);
	kprintf("Data Begin: %d\n", block->DataBegin);
	kprintf("Num Blocks: %d\n", block->NumBlocks);
	kprintf("Free Blocks: %d\n", block->FreeBlocks);
	kprintf("Sectors Per Block: %d\n", block->SectorsPerBlock);
*/
	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
