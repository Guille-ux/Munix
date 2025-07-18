#ifndef DISKI
#define DISKI

#include "general.h"
#include "../include/libcs2.h"
#include "ata.h"
#include "../include/memory.h"

#include <stdint.h>
#include <stddef.h>

typedef enum {
	DISK_UNKNOWN,
	DISK_ATA,
} DiskType;

struct disk_t;

typedef struct disk_t {
	DiskType type;
	void* (*read)(struct disk_t *disk, void *buffer, lba_t lba, uint32_t n);
	void (*write)(struct disk_t *disk, void *buffer, lba_t lba, uint32_t n); 
	union {
		ata_device_t *ata;
		// No Se, supongo que añadire más
	} as;
} disk_t;

void initAtaDisk(ata_device_t *device, disk_t *disk_interface);
void* ataDiskIRead(disk_t *disk, void *buffer, lba_t lba, uint32_t n);
void ataDiskIWrite(disk_t *disk, void *buffer, lba_t lba, uint32_t n);

#endif
