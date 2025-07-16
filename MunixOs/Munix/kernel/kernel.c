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
#include "mbash/lexer.h"
#include "mbash/mbtype.h"
#include "mbash/parser.h"
#include "mbash/eval.h"

#define MAX_LOG_LEN 33
#define MAX_LOGS 512

char *log[MAX_LOGS];
/*
#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

// Sysarena Things
/*
#define MAX_ARENAS 1024*10
#define MEM_SIZE 1024*1024*10

Arena karenas[MAX_ARENAS];
uint8_t memory_pool[MEM_SIZE];
ArenaManager manager;
/*

*/
#define ALL_SIZE 1024*1024*32  // 32MB heap size

free_node *my_free_list[64];
size_t size=ALL_SIZE;
char heap_start[ALL_SIZE];
int mini_order=5;  // 4KB minimum block size (2^12)
int maxi_order=25;  // 32MB maximum block size (2^25)

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
	config_stdmem_buddy((void*)heap_start, ALL_SIZE, mini_order, ((free_node***)&my_free_list));
	libcs_mem_init(stdmem_interface.kmalloc, stdmem_interface.kfree);
	config_klog_interface();
	stdlog_interface.init(buffer_log, MAX_LOGS, MAX_LOG_LEN);
	gdt_init();
	idt_init();  // Remapear PIC a offsets 0x20 y 0x28
	timer_init(100);
	set_kb_spec_1();
	set_kb_layout(&layout_en_US);
	ps2_init(0x01);
	//config_stdmem_buddy((void*)heap_start, ALL_SIZE, mini_order, ((free_node***)&my_free_list));
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
	EvalCtx *global_ctx=newShellCtx();
	//debug=true;
	kprintf("~ MunixOs ~\n");
	Token *t_buff = (Token*)kmalloc(sizeof(Token)*64);
	kprintf("SOCORRO: %d\n", (int)t_buff);
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
			//minim((const char*)shell_buffer);
			lexer_init(shell_buffer);
			lexen(t_buff, 64);
			parser_init(t_buff);
			ASTNode *tree = parse();
			eval(tree, global_ctx);
			parser_free_ast(tree);
			memset((void*)shell_buffer, '\0', SHELL_BUFFER_SIZE);
			shell_index=0;
			shell_update();
			send=false;
		}
		__asm__ volatile("sti");
		__asm__ volatile("hlt");
	} // bucle infinito
}
