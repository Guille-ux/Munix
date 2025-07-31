#ifndef PARTITIONS_H
#define PARTITIONS_H

//#include "mbr.h"
#include "../include/libcs2.h"
#include "../disk/diski.h"
#include "../disk/general.h"
#include "../include/memory.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PARTITIONS 128
#define MAX_DISKS 16

/*
 * Estos son para MBR, y el kernel tambien los usara internamente
 * al menos mientras no tengo GPT, luego podria cambiar alguna estructura o
 * seguir usando esto y añadir un campo opcional que solo se use en GPT
 *
 */
typedef enum {
	FS_TYPE_UNKNOWN=0x00,
	FS_TYPE_FAT12=0x01,
	FS_TYPE_FAT16_LESS_32MB=0x04,
	FS_TYPE_FAT16_GREATER_32MB=0x06,
	FS_TYPE_FAT32_LBA=0x0C,
	FS_TYPE_FAT32_CHS=0x0B,
	FS_TYPE_NTFS=0x07,
	FS_TYPE_LINUX=0x83, // All Linux FS uses the same code xD
	FS_TYPE_LINUX_SWAP=0x82,
	FS_TYPE_MFS=0x0D,
} fs_type_t;

struct kernel_disk_t;

typedef struct {
	uint8_t id; // id único, para DENTRO del disco, realmente no se, xD

	lba_t lba_start;
	
	uint64_t total_sectors;
	
	fs_type_t fs_type;
	
	bool bootable;

	disk_t *parent_disk; // disco en el que esta
} partition_t;

// Ok, Esta es la interfaz de tabla de particiones del kernel
/* 
 * Principalmente voy a tomer prestado `partition_t`
 * y alguna cosa más
 * supongo que esto añadire flexibilidad a la hora de añadir GPT, si es que
 * lo añado claro
 * 
 */

typedef struct {
	partition_t partitions[MAX_PARTITIONS];
	uint8_t count;
} partition_list_t;

typedef struct {
	const char *name;
	partition_list_t* (*detect_partitions)(disk_t *disk);
	bool (*is_valid)(disk_t *disk);
} partition_table_driver_t;



/* Futuro...
 *
 * typedef struct {...} format_driver_t;
 *
 */

/*
 * Función auxiliar para escribir usando la dirección relativa, no usa diski
 * lee la partición para obtener donde empieza etc, tambien controla que no 
 * se salga del rango, además tambien hay otra para leer
 *
 */

void readPartition(partition_t *partition, void *buffer, lba_t where, size_t n);
void writePartition(partition_t *partition, void *buffer, lba_t where, size_t n);

#endif
