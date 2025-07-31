#ifndef PARTITION_MANAGER
#define PARTITION_MANAGER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "partition_mng.h"
#include "partitions.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include "../disk/disk_manager.h"
#include "../disk/diski.h"
#include "../disk/general.h"
#include "../include/low_level.h"

typedef struct {
	partition_table_driver_t *drivers[2]; // es un array de punteros
	int n_drvs;
	partition_t *partitions;
	int partitions_count;
	int capacity;
} partition_manager_t;

extern partition_manager_t kpartition_manager;

void partition_manager_register_drv(partition_manager_t *part_manager, partition_table_driver_t *driver);
void partitionManagerScanDisk(partition_manager_t *p_manager, disk_t *disk);
void partitionManagerRegisterMemory(partition_manager_t *p_mng, size_t amount);
void remountPartitions(partition_manager_t *p_manager, disk_t *disk);
#endif
