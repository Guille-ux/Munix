#include "disk_manager.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include "diski.h"

disk_t system_disks[MAX_DISKS];
int system_disk_count=0;

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

void registerAtaUnit(pci_dev_header_t *ata_header, uint8_t bus, uint8_t slot, uint8_t func, uint8_t channel, uint8_t drv) {
	ata_device_t *ata_dev=kmalloc(sizeof(ata_device_t));
	
	ata_dev->pci_bus = bus;
	ata_dev->pci_slot = slot;
	ata_dev->function = func;
	ata_dev->drv = drv;
	ata_dev->channel = channel;
	ata_dev->irq_line = ata_header->misc.irq_line;

	uint8_t prog_if = ata_header->common.reg2.prog_if; 
	ata_dev->dma_supported = (prog_if & (1 << 7)) ? true : false;

	// desde que descubri el operador ternario lo uso para cualquier cosa
	ata_dev->is_native = (prog_if & (1 << (channel + 2))) ? true : false;

	if (ata_dev->is_native) {
		// Buscar los puertos en los bars
		// hay que usar ~(uint32_t)0x03 por alguna razón,
		// pero lo dice OsDev, y lo que OsDev dice va a misa
		// me fastidia que si son puertos mapeados a memoria la 
		// mascara cambie, pero es lo que hay
		if (channel == 0) {
			ata_dev->io_base = (uint16_t)(ata_header->bar0 & ~(uint32_t)0x03);
			ata_dev->control_base = (uint16_t)(ata_header->bar1 & ~(uint32_t)0x03);
		} else if (channel == 1) {
			ata_dev->io_base = (uint16_t)(ata_header->bar2 & ~(uint32_t)0x03);
			ata_dev->control_base = (uint16_t)(ata_header->bar3 & ~(uint32_t)0x03);
		}
	} else {
		// en caso de que estemos en compatibilidad los puertos son:
		// 0x1F0 y 0x3F6 para canal primario
		// 0x170 y 0x376 para canal secundario
		if (channel == 0) { // canal primario
			ata_dev->io_base = 0x1F0;
			ata_dev->control_base = 0x3F6;
		} else if (channel == 1) {
			ata_dev->io_base = 0x170;
			ata_dev->control_base = 0x376;
		}
	}

	ata_identify_device(ata_dev);
	initAtaDisk(ata_dev, &system_disks[system_disk_count++]);
}
