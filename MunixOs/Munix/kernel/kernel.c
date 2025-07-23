#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "include/ksysarena.h"
#include "include/buddy_alloc.h"
#include "include/libcs2.h"
#include "include/color.h"
#include "include/idt.h"
#include "include/gdt.h"
#include "include/memory.h"
#include "include/pic.h"
#include "include/timer.h"
#include "include/pic.h"
#include "include/shell.h"
#include "disk/general.h"
#include "disk/diski.h"
#include "partitions/partitions.h"
#include "partitions/mbr.h"
#include "pci/pci.h"
#include "include/low_level.h"
#include "disk/disk_manager.h"

#define MAX_LOG_LEN 33
#define MAX_LOGS 512

char *log[MAX_LOGS];
#define ALL_SIZE 1024*1024*32  // 32MB heap size

free_node *my_free_list[64];
size_t size=ALL_SIZE;
char heap_start[ALL_SIZE];
int mini_order=5;  // 4KB minimum block size (2^12)
int maxi_order=25;  // 32MB maximum block size (2^25)

void kernel_ata_routine(ata_device_t *ata_device, disk_t *disk) {
	ata_device->control_base = 0x3F6;
	ata_device->io_base = 0x1F0;
	ata_device->drv = 0;
	ata_device->channel = 0;
	
	ata_identify_device(ata_device);

	parseAtaIdentify(ata_device);

	initAtaDisk(ata_device, disk);
}

static inline void kernel_init() { // Subrutina para inicializar cosas del kernel
	// Desactivar interrupciones
	__asm__ volatile("cli");
	
	// Inicializar sistema de logs
	void **buffer_log = (void*)log;
	config_klog_interface();
	stdlog_interface.init(buffer_log, MAX_LOGS, MAX_LOG_LEN);

	// Inicializar VGA como stdout
	vga_init();
	vga_start();
	stdout_init_vga();
	enable_stdout();
	kclear();

	// Inicializar la Gestión de memoria dinámica
	config_stdmem_buddy((void*)heap_start, ALL_SIZE, mini_order, ((free_node***)&my_free_list));
	libcs_mem_init(stdmem_interface.kmalloc, stdmem_interface.kfree);
	
	// Inicializar GDT y IDT

	gdt_init(); // Cargar tabla de descriptores globales (GDT init)
	
	idt_init(); // Remapear PIC a offsets 0x20 y 0x28 (IDT init)
	
	// Inicializar hardware más especificamente, IRQs
	timer_init(100); // Inicializar temporizador
	set_kb_spec_1(); // Inicializar teclas especiales
	set_kb_layout(&layout_en_US); // seleccionar teclado en_US
	ps2_init(0x01); // inicializar teclado PS/2 con scanset 1

	// barrer el pci en busca de dispositivos
	sweepPCI(false);

	// Activar Interrupciones
	__asm__ volatile("sti");
}

void kernel_main() {
	kernel_init();

	kprintf("~ MunixOs ~\n");
	
	shellEntry();

	while (true) {	
		__asm__ volatile("hlt");
	} // bucle infinito
}
