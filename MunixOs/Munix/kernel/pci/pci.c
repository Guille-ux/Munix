#include "pci.h"

// From OsDev

uint16_t pciCfgReadWord(pci_addr_t addr) {
	uint32_t address;
	uint32_t lbus = (uint32_t)addr.bus;
	uint32_t lslot = (uint32_t)addr.slot;
	uint32_t lfunc = (uint32_t)addr.func;
	uint8_t offset = addr.offset;

	uint16_t tmp=0;

	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | offset & 0xFC | (1 << 31));

	outl(PCI_CONFIG_ADDR, address);

	tmp = (uint16_t)(inl(PCI_CONFIG_DATA) >> ((offset & 2)*8) & 0xFFFF);
}

uint16_t auxPciCfgRWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	pci_addr_t addr;
	addr.bus = bus;
	addr.slot = slot;
	addr.func = func;
	addr.offset = offset;
	return pciCfgReadWord(addr);
}

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

void sweepPCI(bool debug) {
	uint8_t *tmp_mem = (uint8_t*)kmalloc(64*sizeof(uint8_t));
	for (int a=0;a<256;a++) {
		for (int b=0;b<32;b++) {
			//if (debug) {
			//	kprintf("[PCI] : Looking on Bus -> %d, Slot -> %d\n", a, b);
			//}
			readPCIDevCfg(a, b, 0, tmp_mem);
			pci_common_header_t *common_h = (pci_common_header_t*)tmp_mem;

			// la primera función
			if (common_h->reg0.vendor_id == 0xFFFF) {
				continue;
			}
			pciParseHeader(tmp_mem, debug);

			if (common_h->reg3.header_type & (1 << 7)) {
				for (int c=1;c<8;c++) {
					readPCIDevCfg(a, b, c, tmp_mem);
					pciParseHeader(tmp_mem, debug);
				}
			}
		}
	}
	kfree(tmp_mem);
}

void readPCIDevCfg(uint8_t bus, uint8_t slot, uint8_t func, void *buffer) {
	uint32_t *b = (uint32_t*)buffer;
	for (int i = 0;i<64;i+=4) {
		b[i/4] = pciCfgReadD(bus, slot, func, i);
	}
}

uint32_t pciCfgReadD(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | (1 << 31); // construyendo el address
	outl(PCI_CONFIG_ADDR, address);
	return inl(PCI_CONFIG_DATA);
}

void pciParseHeader(void *buffer, bool debug) {
	pci_common_header_t *common_h=(pci_common_header_t*)buffer;
	
	// datos básicos necesarios
	uint16_t vendor_id = common_h->reg0.vendor_id;
	uint16_t device_id = common_h->reg0.device_id;
	uint8_t class_code = common_h->reg2.class_code;
	uint8_t subclass_code = common_h->reg2.subclass;
	uint8_t type = common_h->reg3.header_type & ~(1 << 7);

	if (vendor_id == 0xFFFF) return;
	if (debug) {
		kprintf("[PCI]: Device Found: \n");
		kprintf("\t -> Vendor ID : 0x%x\n", vendor_id);
		kprintf("\t -> Device ID : 0x%x\n", device_id);
		kprintf("\t -> Class Code : 0x%x:0x%x\n", class_code, subclass_code);
		kprintf("\t -> Header Type : 0x%x\n", type);
	}
	switch (type) {
		case 0x00: {
			// el tipo 0x00 sera el primero en soportarse
			pci_dev_header_t *header = (pci_dev_header_t*)buffer;
			if (class_code == 0x01 && subclass_code == 0x01) {
				if (debug) kprintf("[PCI]: IDE/ATA Found!\n");

			}
			break;
			   }
		case 0x01: {
			return;
			break;
			   }
		case 0x02: {
			return;
			break;
			   }
	}
}
