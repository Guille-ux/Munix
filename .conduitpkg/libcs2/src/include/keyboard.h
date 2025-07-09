#ifndef KEYBOARD_CONTROLLER
#define KEYBOARD_CONTROLLER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PS2_DATA 0x60
#define PS2_STATUS 0x64

// PS/2 COMMANDS
#define PS2_DISABLE_FIRST 0xAD
#define PS2_DISABLE_SECOND 0xA7
#define PS2_ENABLE_FIRST 0xAE
#define PS2_ENABLE_SECOND 0xA8

#define PS2_READ_CONFIG_BYTE 0x20
#define PS2_WRITE_CONFIG_BYTE 0x60

#define PS2_SELF_TEST 0xAA
#define PS2_TEST_PASSED 0x55

#define PS2_RESET 0xFF
#define PS2_ACK 0xFA

#define PS2_SCANCODE_SET 0xF0 // select!


extern uint8_t scancode;
extern char final_character;
extern bool mouse;
extern bool shift_pressed;
extern bool caps_lock;
extern bool num_lock;
extern bool win;
extern bool ctrl;
extern bool alt;
extern bool char_out;

#define PS2_BACKSPACE 0x66
// para teclados INTEGRADOS, como comprenderas cuando el teclado es bluetooth o es USB, las cosas se complican un poco más
typedef struct {
	uint16_t data_port;
	uint16_t status_port;
	void (*handle)(void);
} i_keyboard_t;

extern i_keyboard_t keyboard_interface;
void kb_common_handler();
void keyboard_handler();

void ps2_init(uint8_t scan_set);

void ps2_handle();

#endif
