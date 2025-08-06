#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/shell.h"
#include "init/init.h"
#include "fs/fs.h"



void kernel_main() {
	kernel_init();

	kprintf("~ MunixOs ~\n");

	__asm__ volatile("cli");
	lba_t start = {.lo = 0, .hi = 0};
	int i=0;
	
	kprintf("Writing to disk %d\n", i);
	disk_t *disk = &system_disks[i];
	//MBRformatMFS(disk, 0, 4, uint64_2_lba(1), 2047);
		
	explorer_t explorer;
	i=0;
	void *ret=NULL;
	do {
		ret = mfs_init_explorer(&(kpartition_manager.partitions[i++]), &explorer, 0, 0);
	} while (ret==NULL && i < kpartition_manager.partitions_count);
	kprintf("Partition: %d\n", i-1);
	kprintf("N. Partitions: %d\n", kpartition_manager.partitions_count);
	kprintf("Pwd: %s\n", explorer.path);
	
	
	__asm__ volatile("sti");

	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
