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
		
	explorer_t explorer;
	int i=0;
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
