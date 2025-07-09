#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/ksysarena.h"
#include "include/libcs2.h"
#include "include/color.h"
#include "include/idt.h"
#include "include/gdt.h"
#include "include/memory.h"
#include "include/pic.h"
#include "include/timer.h"
#include "include/pic.h"
#include "include/shell.h"
#include "minim/minim.h"

#define MAX_LOG_LEN 33
#define MAX_LOGS 512

char *log[MAX_LOGS];

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

// Sysarena Things
#define MAX_ARENAS 16
#define MEM_SIZE 32

Arena karenas[MAX_ARENAS];
uint8_t memory_pool[MEM_SIZE];
ArenaManager manager;

void kernel_main() {
	__asm__ volatile("cli");
	void **buffer_log = (void*)log;
	vga_init();
	vga_start();
	stdout_init_vga();
	enable_stdout();
	kclear();
	// Verificar inicialización del sistema de impresión
	if (!stdout_interface.active) {
		vga_init(); // Reintentar inicialización VGA
		stdout_init_vga();
	}
	config_stdmem_sysarena(&manager, karenas, memory_pool, MEM_SIZE, MAX_ARENAS);
	config_klog_interface();
	stdlog_interface.init(buffer_log, MAX_LOGS, MAX_LOG_LEN);
	gdt_init();
	idt_init();  // Remapear PIC a offsets 0x20 y 0x28
	timer_init(100);
	set_kb_spec_1();
	set_kb_layout(&layout_en_US);
	ps2_init(0x01);
	/*__asm__ volatile("cli");
	__asm__ volatile("sti");
	*/
	__asm__ volatile("sti");

	/*
	keyboard_interface.handle=ps2_handle;
	keyboard_interface.backspace=0x66;
	keyboard_interface.data_port=0x60;
	keyboard_interface.status_port=0x64;
	*/

	kprintf("~ MunixOs ~\n");
	shell_buffer = (char*)kmalloc(SHELL_BUFFER_SIZE);
	// Una vez finalizada la inicialización activamos los interrupts
	memset((void*)shell_buffer, '\0', SHELL_BUFFER_SIZE);
	shell_update();
	while (true) {
		__asm__ volatile("cli");
		if (shell_event) {
			push_to_buffer(final_character);
			shell_update();
			shell_event=false;
		} else if (backspace) {
			pop_from_buffer();
			shell_update();
			backspace=false;
		} else if (send) {
			kprintf("\n");
			// temporal...
			minim((const char*)shell_buffer);
			memset((void*)shell_buffer, '\0', SHELL_BUFFER_SIZE);
			shell_index=0;
			shell_update();
			send=false;
		}
		__asm__ volatile("sti");
		__asm__ volatile("hlt");
	} // bucle infinito
}
