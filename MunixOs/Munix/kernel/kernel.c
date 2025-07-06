#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/zynk.h"
#include "include/color.h"
#include "include/idt.h"
#include "include/gdt.h"

void kernel_main() {
	vga_init();
	vga_start();
	stdout_init_vga();
	enable_stdout();
	kclear();
	gdt_init();
	idt_init();
	while (true) {

	} // bucle infinito
}
