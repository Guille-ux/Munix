#ifndef PARTITIONS_H
#define PARTITIONS_H

#include "mbr.h"
#include "../include/libcs2.h"
#include "../disk/diski.h"
#include "../disk/general.h"
#include "../include/memory.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PARTITIONS 4
#define MAX_DISKS 4

typedef enum {
	FS_TYPE_UNKNOWN,
	FS_TYPE_FAT12,
	FS_TYPE_FAT16,
	FS_TYPE_FAT32,
	FS_TYPE_NTFS,
	FS_TYPE_EXT2,
} fs_type_t;

struct kernel_disk_t;

typedef struct {
	uint8_t id; // id único
	
	lba_t lba_start;
	
	size_t total_sectors;
	
	uint8_t partition_type;
	
	fs_type_t fs_type;
	
	bool bootable;

	disk_t *parent_disk; // disco en el que esta
} partition_t;

typedef struct kernel_disk_t {
	uint8_t disk_idx;
	disk_t *disk;
	uint8_t num_partitions;
	partition_t partitions[MAX_PARTITIONS];
} kernel_disk_t;

extern kernel_disk_t kernel_disks[MAX_DISKS];
extern uint8_t mapped_disks;

#endif
