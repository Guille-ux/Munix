#include "mbr.h"
#include "partitions.h"
#include "../disk/general.h"
#include "../include/libcs2.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool isMasterBootRecord(void *sector) {
	mbr_t *info = (mbr_t*)sector;
	if (info->data.mbr_sign==MBR_SIGN) {
		kprintf("[MPART]: Master Boot Record found!\n");
		return true;
	}
	return false;
}

void parseMasterBootRecord(void *sector) {
	mbr_t *info = (mbr_t*)sector;
	kprintf("[MPART]: Printing Information...\n");
	kprintf("[MBR]: Disk Sign: 0x%x \n", info->data.disk_sign); // Imprimir firma del disco
	for (int i=0;i<4;i++) {
		printMBRPartition(&info->data.partitions[i]);
	}
	kprintf("[MBR]: MBR Sign: 0x%x \n", info->data.mbr_sign);
}



void printMBRPartition(mbr_partition_t *part) {
	kprintf("[MBR]: Attribute: 0x%x \n", part->attr);
	kprintf("[MBR]: Partition Start (CHS): \n");
	printCHS(part->partition_start);
	kprintf("[MBR]: Partition Type: 0x%x \n", part->type);
	kprintf("[MBR]: Partition End (CHS): \n");
	printCHS(part->last_sector);
	kprintf("[MBR]: LBA Start: %lu \n", part->lba_start);
	kprintf("[MBR]: Number of Sectors: %lu \n", part->size);
}
