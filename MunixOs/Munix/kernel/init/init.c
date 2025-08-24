#include "init.h"

// para los handles
handle_t kernel_handles[N_HANDLES];

// para stdlog
char *log[MAX_LOGS];

// para malloc y free
free_node *my_free_list[64];
size_t size=ALL_SIZE;
char heap_start[ALL_SIZE];
int mini_order = 1;
int maxi_order = 26;


void kernel_init() { // Subrutina para inicializar cosas del kernel
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

	// inicializar gestión de handles
	register_handle_chain(kernel_handles, N_HANDLES);

	// Inicializar GDT y IDT

	gdt_init(); // Cargar tabla de descriptores globales (GDT init)
	
	idt_init();     // inicializar la interrupt descriptor table
			// remapea PIC a offsets 0x20 y 0x28 (IDT init)
	
	// Inicializar hardware más especificamente, IRQs
	timer_init(100); // Inicializar temporizador
	set_kb_spec_1(); // Inicializar teclas especiales
	set_kb_layout(&layout_en_US); // seleccionar teclado en_US
	ps2_init(0x01); // inicializar teclado PS/2 con scanset 1

	// barrer el pci en busca de dispositivos
	sweepPCI(false);
	init_disk_subsystem();

	// Inicializar el disco camaradas, digo inicializando particiones!
	scanByDetectedDisks();

	// Activar Interrupciones
	__asm__ volatile("sti");
}

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

void init_disk_subsystem() {
	/*
	 * Aqui va la inicialización del subsistema de disco
	 * otra complicación que probablemente sea un poco inútil
	 *
	 */
	partition_table_driver_t *mbr_driver = (partition_table_driver_t*)kmalloc(sizeof(partition_table_driver_t));

	initMbrDriver(mbr_driver); // inicializar el driver MBR
	
	partition_manager_register_drv(&kpartition_manager, mbr_driver);
	partitionManagerRegisterMemory(&kpartition_manager, 128);
}

void scanByDetectedDisks() {
	for (int i=0;i<system_disk_count;i++) {
		partitionManagerScanDisk(&kpartition_manager, &system_disks[i]);
	}
}
