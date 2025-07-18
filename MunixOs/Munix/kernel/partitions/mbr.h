#ifndef MBR_H
#define MBR_H

#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../disk/general.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MBR_SIGN 0x55AA // EL NÚMERO MÁGICO
#define MBR_LOCATION 0 // en LBA
#define MBR_LENGTH 1 // en sectores
#define MBR_DATA 0x1B8 // en que byte empieza lo que no interesa

typedef struct {
	uint8_t attr; // atributos
	chs_t partition_start; // inicio de la partición en chs
	uint8_t type; // tipo de partición
	chs_t last_sector; // último sector en chs
	uint32_t lba_start; // inicio en lba
	uint32_t size; // tamaño en sectores
}  __attribute__((packed)) mbr_partition_t;

typedef struct {
	uint32_t disk_sign; // firma del disco
	uint16_t reserved; // always 0
	mbr_partition_t partitions[4];
	uint16_t mbr_sign;
} __attribute__((packed)) mbr_info_t; // información del master boot record

typedef struct {
	uint8_t code[440];
	mbr_info_t data;
} __attribute__((packed)) mbr_t;

bool isMasterBootRecord(void *sector);
void parseMasterBootRecord(void *sector);
void printMBRPartition(mbr_partition_t *part);

#endif
