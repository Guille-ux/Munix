#include "keyboard.h"
#include "../include/libcs2.h"
#include "../handlers/kb_handler.h"

bool bloq_mayus=false;
bool bloq_num=false;
bool shift=false;
bool alt_gr=false;

// this matrix was made with ai because it was too boring
const unsigned char kbd_es[128][3] = { 
    [0x01] = {  27,     27,      0   }, 
    [0x02] = { '1',    '!',      0   },
    [0x03] = { '2',    '"',    '@'   },
    [0x04] = { '3',   0x27,    '#'   }, 
    [0x05] = { '4',    '$',    '~'   },
    [0x06] = { '5',    '%',      0   },
    [0x07] = { '6',    '&',    '¬'   },
    [0x08] = { '7',    '/',      0   },
    [0x09] = { '8',    '(',      0   },
    [0x0A] = { '9',    ')',      0   },
    [0x0B] = { '0',    '=',      0   },
    [0x0C] = { '\'',   '?',      0   },
    [0x0D] = { '¡',    '¿',      0   },
    [0x0E] = { '\b',   '\b',     0   }, 
    [0x0F] = { '\t',   '\t',     0   }, 
    [0x10] = { 'q',    'Q',      0   },
    [0x11] = { 'w',    'W',      0   },
    [0x12] = { 'e',    'E',    '€'   },
    [0x13] = { 'r',    'R',      0   },
    [0x14] = { 't',    'T',      0   },
    [0x15] = { 'y',    'Y',      0   },
    [0x16] = { 'u',    'U',      0   },
    [0x17] = { 'i',    'I',      0   },
    [0x18] = { 'o',    'O',      0   },
    [0x19] = { 'p',    'P',      0   },
    [0x1A] = { '`',    '^',    '['   },
    [0x1B] = { '+',    '*',    ']'   },
    [0x1C] = { '\n',   '\n',     0   }, 
    [0x1E] = { 'a',    'A',      0   },
    [0x1F] = { 's',    'S',      0   },
    [0x20] = { 'd',    'D',      0   },
    [0x21] = { 'f',    'F',      0   },
    [0x22] = { 'g',    'G',      0   },
    [0x23] = { 'h',    'H',      0   },
    [0x24] = { 'j',    'J',      0   },
    [0x25] = { 'k',    'K',      0   },
    [0x26] = { 'l',    'L',      0   },
    [0x27] = { 'ñ',    'Ñ',      0   },
    [0x28] = { '{',    '¨',    '{'   },
    [0x29] = { 'º',    'ª',    '\\'  },
    [0x2B] = { 'ç',    'Ç',    '}'   }, 
    [0x2C] = { 'z',    'Z',      0   },
    [0x2D] = { 'x',    'X',      0   },
    [0x2E] = { 'c',    'C',      0   },
    [0x2F] = { 'v',    'V',      0   },
    [0x30] = { 'b',    'B',      0   },
    [0x31] = { 'n',    'N',      0   },
    [0x32] = { 'm',    'M',      0   },
    [0x33] = { ',',    ';',      0   },
    [0x34] = { '.',    ':',      0   },
    [0x35] = { '-',    '_',      0   },
    [0x39] = { ' ',    ' ',      0   }, 
    [0x56] = { '<',    '>',    '|'   } 
};

void init_kb() {
	// de momento dejamos solo la instalación del teclado ps2
	init_ps2_handler(&ps2_handler);
	kprintf("[ KEYBOARD HANDLER INSTALLED ]\n");
}

uint16_t kgetchar() {
	
		uint16_t key = buffer_pop();
		if (key & SPECIAL_KEY) {
			if (key & KEY_BREAK) {
				key = key & ~KEY_BREAK;
				if (key == KEY_ALT_GR) {
					alt_gr = false;
				} 
				return key;
			}
			if (key == KEY_ALT_GR) {
				alt_gr = true;
				
			} 
			return key;
		} else if (key & KEY_BREAK) {
			 if (key == KEY_BREAK | KEY_LEFT_SHIFT || key == KEY_BREAK | KEY_RIGHT_SHIFT) {
					shift = false;
			}
			return key;
		} else {
			if (key == KEY_LEFT_SHIFT) {
				shift = true;
				return key | KEY_NOT_PRINTABLE;
			} else if (key == KEY_RIGHT_SHIFT) {
				shift = true;
				return key | KEY_NOT_PRINTABLE;
			} else if (key == KEY_BLOQ_MAYUS) {
				bloq_mayus = !bloq_mayus;
				return key | KEY_NOT_PRINTABLE;
			} else if (key == KEY_BLOQ_NUM) {
				bloq_num = !bloq_num;
				return key | KEY_NOT_PRINTABLE;
			}
			// lógica para procesar el teclado ouch
			char index = 0;
			if (bloq_mayus == true) {
				index = 1;
			}
			if (shift == true) {
				index = index ^ 1;
			}
			if (alt_gr == true) {
				index = 2;
			}
			return (kbd_es[key][index] == 0) ? key : kbd_es[key][index];
		}
}
