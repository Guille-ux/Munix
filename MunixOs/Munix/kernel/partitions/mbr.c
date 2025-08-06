#include "mbr.h"
#include "partitions.h"
#include "partition_mng.h"
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
		kprintf("[MBR]: Valid Table\n");
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
	uint8_t *buffer = (uint8_t*)kmalloc(512);
	__asm__ volatile("cli");
	mbr_t *mbr = (mbr_t*)buffer;
	disk->read(disk, buffer, uint64_2_lba(0), 1);
	
	for (int i=0;i<4;i++) {
		mbr_partition_t *c_partition=&mbr->data.partitions[i];
		if (c_partition->lba_start > 0 && c_partition->type != 0) { // comprobando si la partición es válida, creo que
													   // así es la convención, o eso creo
			kprintf("[MBR]: Valid Partition\n");
			partition_t *tmp = &part_list->partitions[part_list->count];			
			tmp->id = part_list->count;
			tmp->lba_start = (lba_t){.lo = mbr->data.partitions[i].lba_start};
			tmp->total_sectors = mbr->data.partitions[i].size;
			tmp->fs_type = (fs_type_t)mbr->data.partitions[i].type;
			tmp->bootable = (mbr->data.partitions[i].attr & (1 << 7)) ? true : false;
			tmp->parent_disk = disk;

			part_list->count++;

		} else {
			kprintf("[MBR]: Invalid Partition!\n");
		}
	}
	__asm__ volatile("sti");
	kfree(mbr);
	return part_list;
}

void initMbrDriver(partition_table_driver_t *driver) {
	driver->name = "MBR Driver";
	driver->detect_partitions = mbr_detect_partitions;
	driver->is_valid = mbr_is_valid;
}

void formatMBRreload(disk_t *disk, void *mbr, partition_manager_t *p_mng) {
	lba_t tmp = {.lo=0, .hi=0};
	disk->write(disk, mbr, tmp, 1);
	remountPartitions(p_mng, disk);
}
