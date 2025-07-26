#ifndef FAT12
#define FAT12

#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../diski/diski.h"
#include "../partitions/partitions.h"
#include "../include/low_level.h"
#include <stdint.h>
#include <stddef.h>

#define FAT12_FREE_CLUSTER 0x000
#define FAT12_CLUSTER_END_START 0xFF8
#define FAT12_CLUSTER_END_END 0xFFF
#define FAT12_BAD_CLUSTER 0xFF7
#define FAT12_RESERVED_CLUSTER 0x001
#define FAT12_ENTRY_SIZE 32 // 32 bytes

#define FAT12_IS_END(cluster) (cluster > 0xFF7 && cluster <= 0xFFF)

typedef struct {
	uint8_t JmpBoot[3]; // this is a jump, because the world is complex
	uint8_t OEMname[8]; // quien demonios formateo esto???
	uint16_t BytesPerSector; // bytes por sector, xD
	uint8_t SectorsPerCluster; // sectores por cluster, pero, que es un
				   // cluster???
	uint16_t ReservedSectors;
	uint8_t NumTables;
	uint16_t RootEntries;
	uint16_t TotalSectors; // si esto == 0 entonces estan en el extendido
			       // aunque, esto es rebundante conociendo
			       // la partición
	uint8_t DrvType; // tipo de descriptor de medio, no me importa mucho
	uint16_t SectorsPerFat; // only FAT12 & FAT16
	uint16_t SectorsPerTrack;
	uint16_t HeadsPerDrive;
	uint32_t LbaStart;
	uint32_t GreatSectorCount;
} __attribute__((packed)) fat12_bpb_t;

typedef struct {
	uint8_t unit_number; // Segun osdev es totalmente inútil
	uint8_t reserved; // algo de Güindous NT
	uint8_t sign; // debe ser 0x28 o 0x29
	uint32_t SerialNumber; // se puede ignorar, o eso dice tito OsDev
	uint8_t VolumeTag[11]; // xD vamos a ponerle nombre a la partición
	uint8_t IdString[8]; // la especificación dice que no nos fiemos
			     // wtf, en serio?? para que esta esa cosa
	uint8_t BootCode[448];
	uint16_t BootSignature;
} __attribute__((packed)) fat12_ext_bpb_t

int16_t getFat12Entry(uint32_t cluster, partition_t *partition);
fat12_bpb_t *fat12_get_bpb(partition_t *partition);
uint32_t fat12MaxClusters(fat12_bpb_t *bpb, partition_t *partition);
int fat12ReadClusterChain(uint32_t first_cluster, void *buffer, uint32_t max_size, uint32_t *read, partition_t *partition);
int fat12SetEntry(void *buffer, uint16_t cluster, uint32_t byte_offset, uint16_t new_val);
int16 RemoveFat12Entry(uint32_t cluster, partition_t *partition);
int fat12RemoveClusterChain(uint32 first_cluster, partition_t *partition);


/*
 * Ahora la parte que se preocupa de los directorios
 */



#endif
