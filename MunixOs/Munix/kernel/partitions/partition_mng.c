#include "partition_mng.h"
#include "partitions.h"
#include "../include/memory.h"
#include "../include/libcs2.h"

partition_manager_t kpartition_manager = {.n_drvs=0, .partitions=NULL, .partitions_count=0};

void partition_manager_register_drv(partition_manager_t *part_manager, partition_table_driver_t *driver) {
	part_manager->drivers[part_manager->n_drvs++]=driver;
}

void partitionManagerScanDisk(partition_manager_t *p_manager, disk_t *disk) {
	partition_table_driver_t *driver=NULL;
	for (int i=0;i<p_manager->n_drvs;i++) {
		if (p_manager->drivers[i]!=NULL) {
			driver=p_manager->drivers[i];
			if (!driver->is_valid(disk)) {
				driver=NULL;
				continue;
			}
			kprintf("[PARTITION MANAGER]: Detected by %s\n", driver->name);
			break;
		}
	}
	if (driver==NULL) {
		kprintf("[PARTITION MANAGER]: Unknown partition table!\n");
		return;
	}
	partition_list_t *tmp = driver->detect_partitions(disk);
	if (p_manager->partitions_count + tmp->count > p_manager->capacity) {
		kprintf("[PARTITION MANAGER]: Out of PRE-Allocated Memory\n");
		return;
	}
	for (int i=0;i<tmp->count;i++) {
		p_manager->partitions[p_manager->partitions_count++]=tmp->partitions[i];
	}
	kfree(tmp);
	kprintf("[PARTITION MANAGER]: Partitions Added to global Table\n");
}
void partitionManagerRegisterMemory(partition_manager_t *p_mng, size_t amount) {
	if (p_mng->partitions!=NULL) {
		kfree(p_mng->partitions);
		p_mng->partitions_count=0;
	}
	p_mng->capacity=amount;
	p_mng->partitions=(partition_t *)kmalloc(sizeof(partition_t)*amount);

}
