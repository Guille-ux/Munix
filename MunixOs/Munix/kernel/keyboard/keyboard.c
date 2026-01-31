#include "keyboard.h"
#include "../include/libcs2.h"
#include "../handlers/kb_handler.h"

void init_kb() {
	// de momento dejamos solo la instalación del teclado ps2
	init_ps2_handler(&ps2_handler);
	kprintf("[ KEYBOARD HANDLER INSTALLED ]\n");
}
