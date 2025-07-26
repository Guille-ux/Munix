#include "diski.h"


void initAtaDisk(ata_device_t *device, disk_t *disk_interface) {
	disk_interface->type = DISK_ATA;
	disk_interface->read=ataDiskIRead;
	disk_interface->write=ataDiskIWrite;
	disk_interface->as.ata=device;
}

void* ataDiskIRead(disk_t *disk, void *buffer, lba_t lba, uint32_t n) {
	uint64_t tmp=0;
	void *b = buffer;
	while (n > 0) {
		ataReadLBA2(disk->as.ata, lba, buffer, n & 0xFFFF);
		tmp = lba2uint64(lba);
		tmp += (n & 0xFFFF);
		b = (void*)((uintptr_t)b + (n & 0xFFFF) * 512);
		lba = uint64_2_lba(tmp);
		n -= (n & 0xFFFF);
	}
	return buffer;
}

void ataDiskIWrite(disk_t *disk, void *buffer, lba_t lba, uint32_t n) {
	uint64_t tmp=0;
	void *b = buffer;
	while (n > 0) {
		ataWriteLBA2(disk->as.ata, lba, buffer, n & 0xFFFF);
		tmp = lba2uint64(lba);
		tmp += (n & 0xFFFF);
		b = (void*)((uintptr_t)b + (n & 0xFFFF) * 512);	
		lba = uint64_2_lba(tmp);
		n -= (n & 0xFFFF);
	}
}
