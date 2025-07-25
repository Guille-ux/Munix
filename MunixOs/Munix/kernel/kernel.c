#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/shell.h"
#include "init/init.h"

void kernel_main() {
	kernel_init();

	kprintf("~ MunixOs ~\n");
	
	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
