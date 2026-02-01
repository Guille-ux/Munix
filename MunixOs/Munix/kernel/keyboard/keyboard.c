#include "keyboard.h"
#include "../include/libcs2.h"
#include "../handlers/kb_handler.h"

bool bloq_mayus=false;
bool bloq_num=false;
bool shift=false;
bool alt_gr=false;

kb_entry_t *kb_layout;

kb_entry_t kb_es_layout[128];

void init_kb() {
	// de momento dejamos solo la instalación del teclado ps2
	init_ps2_handler(&ps2_handler);
	kprintf("[ KEYBOARD HANDLER INSTALLED ]\n");
}

uint16_t kgetchar() {
	uint16_t key = buffer_pop();
	if (key & SPECIAL_KEY) {
		return key;
	}
}
