#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/libcs2.h"
#include "include/zynk.h"
#include "include/color.h"

void kernel_main() {
	vga_clear(stdcolor);
	vga_print_string(0, 0, "Hello World!", (0x0F));
	while (true) {} // bucle infinito
}
