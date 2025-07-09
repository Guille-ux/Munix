#ifndef KEYMAPS_H
#define KEYMAPS_H

#include <stdint.h>

#define KC_LSHIFT_2       0x12
#define KC_RSHIFT_2       0x59
#define KC_LCTRL_2        0x14
#define KC_LALT_2         0x11
#define KC_RALT_EXT_2     0xE011
#define KC_LSUPER_EXT_2   0xE02F
#define KC_RSUPER_EXT_2   0xE027
#define KC_CAPSLOCK_2     0x58
#define KC_NUMLOCK_2      0x77
#define KC_SCROLLLOCK_2   0x7E
#define KC_BACKSPACE_2    0x66
#define KC_ENTER_2        0x5A
#define KC_TAB_2          0x0D
#define KC_ESC_2          0x76

typedef struct {
	uint8_t lshift;
	uint8_t rshift;
	uint16_t lctrl;
	uint16_t lalt;
	uint8_t lsuper;
	uint8_t capslock;
	uint8_t numlock;
	uint8_t scrolllock;
	uint8_t backspace;
	uint8_t enter;
	uint8_t tab;
	uint8_t esc;
	uint8_t lbshift;
	uint8_t rbshift;
} kb_spec;

typedef struct {
	const uint8_t *normal_map;
	const uint8_t *shift_map;
	const uint8_t *caps_map;
	const uint8_t *num_map;
} kb_t;

extern kb_spec special_layout;
extern char kb_prefix;
extern kb_t layout_en_US;
extern kb_t *keyboard_layout;
void set_kb_layout(kb_t *kb_layout);
void set_kb_spec_1(void); // para el set 1 de scancodes

#endif
