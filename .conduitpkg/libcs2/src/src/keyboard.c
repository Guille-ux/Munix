#include "../include/keymaps.h"
#include "../include/io.h"
#include "../include/keyboard.h"
#include "../include/printf.h"
// atención, solo para teclados integrados



const uint8_t _kbd_us_normal_map[256] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, // 0x00-0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's', // 0x10-0x1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', // 0x20-0x2F
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40-0x4F (NumPad)
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50-0x5F
    
};


const uint8_t _kbd_us_shift_map[256] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, // 0x00-0x0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S', // 0x10-0x1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', // 0x20-0x2F
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40-0x4F (NumPad)
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50-0x5F
    // ...
};


const uint8_t _kbd_us_caps_map[256] = {
    
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    
};


const uint8_t _kbd_us_num_map[256] = {};


// Definición del layout en_US
kb_t layout_en_US = {
    .normal_map = _kbd_us_normal_map,
    .shift_map = _kbd_us_shift_map,
    .caps_map = _kbd_us_caps_map,
    .num_map = _kbd_us_num_map,
};

kb_t *keyboard_layout;

kb_spec special_layout;

void set_kb_layout(kb_t *kb_layout) {
	keyboard_layout = kb_layout;
}

i_keyboard_t keyboard_interface;
char final_character;
bool mouse=false;
bool shift_pressed=false;
bool caps_lock=false;
bool num_lock=false;
bool win=false;
bool ctrl=false;
bool alt=false;
char kb_prefix=0;
uint8_t scancode=0;
bool char_out=false;


/*bool ps2_controller_exists() {
    outb(PS2_STATUS, 0xAD); // Disable first port
    outb(PS2_STATUS, 0xA7); // Disable second port
    io_wait();
    
    uint8_t config = inb(PS2_STATUS);
    return !(config & 0x04); // Bit 2 = 0 means controller exists
}*/

void ps2_flush() {
    while (inb(PS2_STATUS) & 0x01) {
        inb(PS2_DATA);
    }
}

bool ps2_wait_for_write(uint32_t timeout) {
    uint32_t start = 0;
    while (inb(PS2_STATUS) & 0x02) {
        if (++start > timeout) return false;
        io_wait();
    }
    return true;
}

bool ps2_wait_for_read(uint32_t timeout) {
    uint32_t start = 0;
    while (!(inb(PS2_STATUS) & 0x01)) {
        if (++start > timeout) return false;
        io_wait();
    }
    return true;
}

bool ps2_send_cmd(uint8_t cmd) {
    if (!ps2_wait_for_write(1000)) return false;
    outb(PS2_STATUS, cmd);
    return true;
}

bool ps2_send_data(uint8_t data) {
    if (!ps2_wait_for_write(1000)) return false;
    outb(PS2_DATA, data);
    return true;
}

bool ps2_expect_ack() {
    if (!ps2_wait_for_read(1000)) return false;
    return inb(PS2_DATA) == PS2_ACK;
}

// function to set layout for scancode set 1 (me refiero a las teclas especiales de el struct kb_spec)


void ps2_init(uint8_t scan_set) {
    // Verify PS/2 controller exists
    /*if (!ps2_controller_exists()) {
        kprintf("PS2: No controller found\n");
        return;
    }*/

    // Disable both devices
    ps2_send_cmd(PS2_DISABLE_FIRST);
    ps2_send_cmd(PS2_DISABLE_SECOND);
    ps2_flush();

    // Read and update config byte
    if (!ps2_send_cmd(PS2_READ_CONFIG_BYTE)) return;
    if (!ps2_wait_for_read(1000)) return;
    uint8_t config = inb(PS2_DATA);

    config |= 0x01;  // Enable first port interrupt
    config &= ~0x02; // Disable second port
    config &= ~0x40; // Disable translation

    if (!ps2_send_cmd(PS2_WRITE_CONFIG_BYTE)) return;
    if (!ps2_send_data(config)) return;

    // Test controller
    if (!ps2_send_cmd(PS2_SELF_TEST)) return;
    if (!ps2_wait_for_read(1000)) return;
    uint8_t test_result = inb(PS2_DATA);
    kprintf("PS2: Controller test %s \n", test_result == PS2_TEST_PASSED ? "passed" : "failed");

    // Enable first port
    ps2_send_cmd(PS2_ENABLE_FIRST);

    // Reset keyboard
    ps2_send_data(PS2_RESET);
    if (!ps2_expect_ack()) {
        kprintf("PS2: Reset failed (no ACK)\n");
        return;
    }

    if (!ps2_wait_for_read(1000)) return;
    uint8_t reset_result = inb(PS2_DATA);
    kprintf("PS2: Reset %s \n", reset_result == 0xAA ? "successful" : "failed");

    ps2_send_data(0xF0); // Set scancode command
    ps2_expect_ack();
    ps2_send_data(scan_set);
    ps2_expect_ack();

    // Setup interface
	keyboard_interface.handle = ps2_handle;
	keyboard_interface.data_port = PS2_DATA;
	keyboard_interface.status_port = PS2_STATUS;
}

void set_kb_spec_1() {
    // rellenar, es necesario rellenar todo el struct
    special_layout.lshift=0x2A;
    special_layout.rshift=0x36;
    special_layout.lctrl=0xE01D;
    special_layout.lalt=0xE038;
    special_layout.enter=0x1C;
    special_layout.backspace=0x0E;
    special_layout.esc=0x01;
    special_layout.tab=0x0F;
    special_layout.lsuper=0xE047;
    special_layout.numlock=0x45;
    special_layout.scrolllock=0x46;
    special_layout.lbshift=0xAA;
    special_layout.rbshift=0xB6;
}


/*
const uint8_t _kbd_us_normal_map[256] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, // 0x00-0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's', // 0x10-0x1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', // 0x20-0x2F
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40-0x4F (NumPad)
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50-0x5F
    
};


const uint8_t _kbd_us_shift_map[256] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, // 0x00-0x0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S', // 0x10-0x1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', // 0x20-0x2F
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40-0x4F (NumPad)
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50-0x5F
    // ...
};


const uint8_t _kbd_us_caps_map[256] = {
    
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    
};


const uint8_t _kbd_us_num_map[256] = {};


// Definición del layout en_US
kb_t layout_en_US = {
    .normal_map = _kbd_us_normal_map,
    .shift_map = _kbd_us_shift_map,
    .caps_map = _kbd_us_caps_map,
    .num_map = _kbd_us_num_map,
};

kb_t *keyboard_layout;



void set_kb_layout(kb_t *kb_layout) {
	keyboard_layout = kb_layout;
}

i_keyboard_t keyboard_interface;
char final_character;
bool mouse=false;
bool shift_pressed=false;
bool caps_lock=false;
bool num_lock=false;
bool win=false;
bool ctrl=false;
bool alt=false;
char kb_prefix;
uint8_t scancode;

void ps2_wait_for_write() {
    while (inb(PS2_STATUS) & 0x02) {
        io_wait();
    }
}

uint8_t ps2_read_data() {
    while (!(inb(PS2_STATUS) & 0x01)) { 
        io_wait(); 
    }
    return inb(PS2_DATA);
}

void ps2_init() {
	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_DISABLE_FIRST);
	io_wait();
	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_DISABLE_SECOND);
	io_wait();

	// cleaning...
	while (inb(PS2_STATUS) & 0x01) {
		inb(PS2_DATA);
	}

	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_READ_CONFIG_BYTE);
	io_wait();
	uint8_t config = inb(PS2_DATA);

	// Enable interrupts of first port, who needs mouse?
	config |= 0x01;
	if (mouse) {
		config |= 0x02;
	} else {
		config &= ~0x02;
	}

	// disabling traduction...
	config &= ~0x40;

	// writing config...
	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_WRITE_CONFIG_BYTE);
	io_wait();
	ps2_wait_for_write();
	outb(PS2_DATA, config);
	io_wait();

	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_SELF_TEST);
	io_wait();
	if (inb(PS2_DATA) == PS2_TEST_PASSED) {
		kprintf("-> PS2 SELF TEST [PASSED]\n");
	} else {
		kprintf("-> PS2 SELF TEST [FAILED]\n");
	}

	ps2_wait_for_write();
	outb(PS2_STATUS, PS2_ENABLE_FIRST); io_wait();
	if (mouse) outb(PS2_STATUS, PS2_ENABLE_SECOND); io_wait();

	// reset keyboard
	ps2_wait_for_write();
	outb(PS2_DATA, PS2_RESET);

	if (inb(PS2_DATA) == PS2_ACK) {
		kprintf("-> PS2 ACK [OK]\n");
	} else {
		kprintf("-> PS2 ACK [ERR]\n");
	}

	// selecting scancode set
	ps2_wait_for_write();
	outb(PS2_DATA, PS2_SCANCODE_SET); io_wait();
	ps2_wait_for_write();
	outb(PS2_DATA, 0x02); io_wait();// selecting scancode set number 2

	keyboard_interface.backspace = PS2_BACKSPACE;
	keyboard_interface.handle = ps2_handle;
	keyboard_interface.data_port = PS2_DATA;
	keyboard_interface.status_port = PS2_STATUS;
}*/

void keyboard_handler() {
	keyboard_interface.handle();
}

void ps2_handle() {
	scancode=inb(PS2_DATA);
	// DEBUG : kprintf("-> PS2 \n\t -> %d  Pressed!\n", (int)scancode);
	kb_common_handler();
}
void kb_common_handler() {
    char_out = !char_out;
	uint16_t full_scancode = scancode;
	bool is_break=false;

	if (scancode==0xE0) {
		kb_prefix = 0xE0;
        char_out = true;
		keyboard_handler();
        return;
	} if (scancode==0xF0) {
		kb_prefix = 0xF0;
        char_out = true;
		keyboard_handler();
        return;
	}

	if (kb_prefix==0xE0) {
		full_scancode = (uint16_t)0xE000 | scancode;
	} else if (kb_prefix==0xF0) {
		is_break = true;

		// no se porque, pero se supone que lo tengo que hacer
		full_scancode = scancode;
	}
	// reiniciar prefijo
	kb_prefix = 0;
    char_out=false;
    if (full_scancode==special_layout.lshift) {
        shift_pressed = true;
    } else if (full_scancode==special_layout.lbshift || full_scancode == special_layout.rbshift) {
        shift_pressed = false;
    } else if (full_scancode==special_layout.lsuper) {
        win = !is_break;
    } else if (full_scancode==special_layout.lalt) {
        alt = !is_break;
    } else if (full_scancode==special_layout.enter) {

    } else if (full_scancode==special_layout.backspace) {

    } else if (full_scancode==special_layout.esc) {

    } else if (full_scancode==special_layout.tab) {

    } else if (full_scancode==special_layout.scrolllock) {

    } else if (full_scancode==special_layout.rshift) {

    } else if (full_scancode==special_layout.rshift) {
        shift_pressed = true; 
    } else if (full_scancode==special_layout.lctrl) {
        ctrl = !is_break; 
    } else if (full_scancode==special_layout.capslock) {
        caps_lock = !caps_lock;
    } else if (full_scancode==special_layout.numlock) {
        num_lock = !num_lock;
    } else {
        char_out=true;
    }

	if (!is_break) {
		final_character = '\0';
		if (shift_pressed) {
    		final_character = keyboard_layout->shift_map[scancode];
		} else {
			final_character = keyboard_layout->normal_map[scancode];
		}

		if (caps_lock && final_character >= 'a' && final_character <= 'z') { 
			final_character -= 32; // convertir a mayuscula
		} else if (caps_lock && final_character >= 'A' && final_character <= 'Z') {
			final_character += 32;
		}
	}

}

