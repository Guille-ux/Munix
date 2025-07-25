#include "mbr.h"
#include "partitions.h"
#include "../disk/general.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool isMasterBootRecord(void *sector) {
	mbr_t *info = (mbr_t*)sector;
	if (info->data.mbr_sign==0xAA55) {
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

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

bool mbr_is_valid(disk_t *disk) {
	uint8_t *info = (uint8_t*)kmalloc(sizeof(uint8_t)*512);
	lba_t lba = {.lo=0, .hi=0};
	disk->read(disk, info, lba, 1);
	if (isMasterBootRecord(info)) {
		kfree(info);
		return true;
	}
	kfree(info);
	return false;
}

partition_list_t *mbr_detect_partitions(disk_t *disk) {
	// asignar espacio para la lista de particiones
	partition_list_t* part_list = (partition_list_t*)kmalloc(sizeof(partition_list_t));
	part_list->count=0;
	// asignar espacio y leer el MBR
	mbr_t *mbr = (mbr_t*)kmalloc(sizeof(mbr_t));
	disk->read(disk, mbr, uint64_2_lba(1), 1);
	
	for (int i=0;i<4;i++) {
		if (mbr->data.partitions[i].lba_start != 0 && mbr->data.partitions[i].size != 0) { // comprobando si la partición es válida, creo que
												   // así es la convención, o eso creo
			partition_t *tmp = &part_list->partitions[part_list->count];			
			tmp->id = part_list->count;
			tmp->lba_start = (lba_t){.lo = mbr->data.partitions[i].lba_start};
			tmp->total_sectors = mbr->data.partitions[i].size;
			tmp->fs_type = (fs_type_t)mbr->data.partitions[i].type;
			tmp->bootable = (mbr->data.partitions[i].attr & (1 << 7)) ? true : false;
			tmp->parent_disk = disk;

			part_list->count++;

		}
	}
	kfree(mbr);
	return part_list;
}

void initMbrDriver(partition_table_driver_t *driver) {
	driver->name = "MBR Driver";
	driver->detect_partitions = mbr_detect_partitions;
	driver->is_valid = mbr_is_valid;
}
