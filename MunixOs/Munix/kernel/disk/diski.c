#include "diski.h"


void initAtaDisk(ata_device_t *device, disk_t *disk_interface) {
	disk_interface->type = DISK_ATA;
	disk_interface->read=ataDiskIRead;
	disk_interface->write=ataDiskIWrite;
	disk_interface->as.ata=device;
}

void* ataDiskIRead(disk_t *disk, void *buffer, lba_t lba, uint32_t n) {
	return ataReadLBA(disk->as.ata, lba, buffer, n & 0xFFFF);
}

void ataDiskIWrite(disk_t *disk, void *buffer, lba_t lba, uint32_t n) {
	ataWriteLBA(disk->as.ata, lba, buffer, n & 0xFFFF);
}
