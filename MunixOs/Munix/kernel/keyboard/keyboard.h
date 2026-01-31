#ifndef MUNIX_KEYBOARD_H
#define MUNIX_KEYBOARD_H

#include "../handlers/kb_handler.h"

// inicialización del nuevo driver de teclado
void init_kb();

// teclas
#define SPECIAL_KEY (1 << 8)

#define KEY_HOME SPECIAL_KEY | 0x47


#endif
