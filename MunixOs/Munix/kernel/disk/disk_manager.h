#ifndef DISK_MANAGER
#define DISK_MANAGER

#include "../pci/pci.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include "diski.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_DISKS 16

extern disk_t system_disks[MAX_DISKS];
extern int system_disk_count;

void registerAtaUnit(pci_dev_header_t *ata_header, uint8_t bus, uint8_t slot, uint8_t func, uint8_t channel, uint8_t drv);

#endif
