#include "../include/keymaps.h"
#include "../include/io.h"
#include "../include/keyboard.h"
#include "../include/printf.h"
// atención, solo para teclados integrados



uint16_t (*getchar)(void);


i_keyboard_t keyboard_interface;
bool mouse=false;
bool caps_lock=false;
bool num_lock=false;




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
	keyboard_interface.data_port = PS2_DATA;
	keyboard_interface.status_port = PS2_STATUS;
}






