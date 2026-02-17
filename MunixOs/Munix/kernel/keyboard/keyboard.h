#ifndef MUNIX_KEYBOARD_H
#define MUNIX_KEYBOARD_H

#include "../handlers/kb_handler.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// inicialización del nuevo driver de teclado
void init_kb();
uint16_t kgetchar();

// teclas
#define SPECIAL_KEY (1 << 8)
#define KEY_BREAK (1 << 7)

#define KEY_HOME SPECIAL_KEY | 0x47
#define KEY_APPS SPECIAL_KEY | 0x5D
#define KEY_ESC 0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB 0x0F
#define KEY_ENTER 0x1C
#define KEY_ALT_GR SPECIAL_KEY | 0x38
#define KEY_BLOQ_MAYUS 0x3A
#define KEY_BLOQ_NUM 0x45
#define KEY_LEFT_SHIFT 0x2A
#define KEY_LEFT_ALT 0x38
#define KEY_RIGHT_SHIFT 0x36

// teclas de función
#define KEY_F1 0x3B
#define KEY_F2 0x3C
#define KEY_F3 0x3D
#define KEY_F4 0x3E
#define KEY_F5 0x3F
#define KEY_F6 0x40
#define KEY_F7 0x41
#define KEY_F8 0x42
#define KEY_F9 0x43
#define KEY_F10 0x44
#define KEY_F11 0x57
#define KEY_F12 0x58

// las flechas
#define KEY_ARROW_UP SPECIAL_KEY & 0x48
#define KEY_ARROW_DOWN SPECIAL_KEY & 0x50
#define KEY_ARROW_RIGHT SPECIAL_KEY & 0x4D
#define KEY_ARROW_LEFT SPECIAL_KEY & 0x4B


typedef struct {
	char normal;
	char shift;
	char alt;
} kb_entry_t;

extern const unsigned char kbd_es[128][3];

#endif
