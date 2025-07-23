#ifndef PCI
#define PCI

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../include/low_level.h"
#include "../include/memory.h"

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC
#define PCI_CONFIG_SIZE 256 // segun osdev cada dispositivo tiene 256 bytes

typedef struct {
	uint8_t bus;
	uint8_t slot;
	uint8_t func;
	uint8_t offset;
} pci_addr_t;

typedef struct {
	struct __attribute__((packed)) {
		uint32_t vendor_id : 16;
		uint32_t device_id : 16;
	} reg0;
	struct __attribute__((packed)) {
		uint32_t command : 16;
		uint32_t status : 16;
	} reg1;
	struct __attribute__((packed)) {
		uint32_t revision_id : 8;
		uint32_t prog_if : 8;
		uint32_t subclass : 8;
		uint32_t class_code : 8;
	} reg2;
	struct __attribute__((packed)) {
		uint32_t cache_line_size : 8;
		uint32_t latency_timer : 8;
		uint32_t header_type : 8;
		uint32_t bist : 8;
	} reg3;
} __attribute__((packed)) pci_common_header_t;

typedef struct {
	pci_common_header_t common;
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	uint32_t cardbus_cis; // xD, el CIS
	struct __attribute__((packed)) {
		uint32_t vendor_id : 16;
		uint32_t id : 16;
	} subsystem;
	uint32_t expansion_rom_addr;
	struct __attribute__((packed)) {
		uint32_t ptr : 8;
		uint32_t reserved : 24;
	} capabilities; // capabilities
	uint32_t reserved;
	struct __attribute__((packed)) {
		uint32_t irq_line : 8;
		uint32_t int_pin : 8;
		uint32_t min_grant : 8; // 1 / 4 microsec
		uint32_t max_lat : 8; // 1 / 4 microsec
	} misc;
} __attribute__((packed)) pci_dev_header_t;

uint32_t pciCfgReadD(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t auxPciCfgRWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pciCfgReadWord(pci_addr_t pci_addr);
void sweepPCI(bool debug);
void readPCIDevCfg(uint8_t bus, uint8_t slot, uint8_t func, void *buffer);
void pciParseHeader(void *buffer, uint8_t bus, uint8_t slot, uint8_t func, bool debug);

#endif
