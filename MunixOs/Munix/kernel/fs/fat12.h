#ifndef FAT12
#define FAT12

#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../disk/diski.h"
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
} __attribute__((packed)) fat12_ext_bpb_t;

int16_t getFat12Entry(uint32_t cluster, partition_t *partition);
fat12_bpb_t *fat12_get_bpb(partition_t *partition);
uint32_t fat12MaxClusters(fat12_bpb_t *bpb, partition_t *partition);
int fat12ReadClusterChain(uint32_t first_cluster, void *buffer, uint32_t max_size, uint32_t *read, uint32_t *expected, partition_t *partition);
int fat12SetEntry(void *buffer, uint16_t cluster, uint32_t byte_offset, uint16_t new_val);
int16_t RemoveFat12Entry(uint32_t cluster, partition_t *partition);
int fat12RemoveClusterChain(uint32_t first_cluster, partition_t *partition);
int fat12AllocClusters(partition_t *partition, uint32_t n, uint32_t *first_cluster);
int fat12SetCluster(partition_t *partition, fat12_bpb_t *bpb, uint32_t cluster, uint32_t new_val);
int fat12WriteClusterChain(partition_t *partition, uint32_t first_cluster, void *buffer, uint32_t n); // n es el número de bytes que vamos a escribir

/*
 * Ahora la parte que se preocupa de los directorios
 */

// Atributos
#define FAT12_ATTR_READ_ONLY 0x01
#define FAT12_ATTR_HIDDEN 0x02
#define FAT12_ATTR_SYSTEM 0x04
#define FAT12_ATTR_VOLUME_ID 0x08
#define FAT12_ATTR_DIR 0x10
#define FAT12_ATTR_ARCHIVE 0x20
#define FAT12_ATTR_LONG_NAME 0x0F

#define FAT12_ENTRY_FREE 0xE5
#define FAT12_LAST_ENTRY 0x00

typedef struct {
	uint8_t name[8];
	uint8_t extension[3];
	uint8_t attr;
	uint8_t flag; // banderas opcionales, no me importan
	uint8_t DirCrtTenth; // es opcional tambien, pero lo siguiente
			     // aunque tambien lo sea, me lo tomare más
			     // en serio
	uint16_t DirCrtTime;
	uint16_t DirCrtDate;
	uint16_t DirLastAccess;
	uint16_t firstClusHI;
	uint16_t DirWrtTime;
	uint16_t DirWrtDate;
	uint16_t firstClusLO;
	uint32_t size; // si es un directorio esto no se incluye
} __attribute__((packed)) fat12_entry_t;

static inline lba_t FAT12getRootDirLba(partition_t *partition, fat12_bpb_t *bpb) {
	uint32_t offset = bpb->ReservedSectors +  bpb->NumTables * bpb->SectorsPerFat;
	return lba2uint64(offset + uint64_2_lba(partition->lba_start));
}

static inline uint32_t FAT12getRootDirSize(fat12_bpb_t *bpb) {
	return (FAT12_ENTRY_SIZE * bpb->RootEntries) / bpb->BytesPerSector;
}

static inline void to83standar(char buffer[11], const char *name) {
	char delimiters[] = {'.'};
	char *n = strdup(name);
	char *nombre = strtok(n, delimiters);
	char *extension = strtok(NULL, delimiters);
	int n_len=0;
	int ext_len=0;
	if (nombre!=NULL) {
		n_len = strlen(nombre);
	} if (extension != NULL) {
		ext_len = strlen(extension);
	}
	int i=0;
	memset(buffer, ' ', 11);
	while (i < n_len) {
		if (nombre[i]<='z' && nombre[i]>='a') {
			buffer[i]=nombre[i] + ('A' - 'a');
		} else if (nombre[i]<='Z' && nombre[i]>='A') {
			buffer[i]=nombre[i];
		} else if (nombre[i]<='9' && nombre[i]>='0') {
			buffer[i]=nombre[i];
		} else if (nombre[i]=='!' ||
			   nombre[i]=='#' ||
			   nombre[i]=='$' ||
			   nombre[i]=='%' ||
			   nombre[i]=='&' ||
			   nombre[i]=='\'' ||
			   nombre[i]=='-' ||
			   nombre[i]=='@' ||
			   nombre[i]=='^' ||
			   nombre[i]=='\\' ||
			   nombre[i]=='_' ||
			   nombre[i]=='~' ||
			   nombre[i]=='`' ||
			   nombre[i]=='(' ||
			   nombre[i]==')' ||
			   nombre[i]=='{' ||
			   nombre[i]=='}') {
			buffer[i]=nombre[i];
		}
		i++;
	}
	if (extension != NULL) {
		for (int a=0;a<ext_len;a++) {
			if (extension[a]<='z' && extension[a]>='a') {
				buffer[a+8]=extension[a] + ('A' - 'a');
			} else if (extension[a]<='Z' && extension[a]>='A') {
				buffer[a+8]=extension[a];
			} else if (extension[a]<='9' && extension[a]>='0') {
				buffer[a+8]=extension[a];
			} else if (extension[a]=='!' ||
				   extension[a]=='#' ||
				   extension[a]=='$' ||
				   extension[a]=='%' ||
				   extension[a]=='&' ||
				   extension[a]=='\'' ||
				   extension[a]=='-' ||
				   extension[a]=='@' ||
				   extension[a]=='^' ||
				   extension[a]=='\\' ||
				   extension[a]=='_' ||
				   extension[a]=='~' ||
				   extension[a]=='`' ||
				   extension[a]=='(' ||
				   extension[a]==')' ||
				   extension[a]=='{' ||
				   extension[a]=='}') {
				buffer[a+8]=extension[a];
			}
		}
	}
	free(n);
}

void FAT12listRootDirectory(partition_t *partition, fat12_bpb_t *bpb);
void FAT12listInDirectory(void *buffer, uint32_t n);
fat12_entry_t *FAT12searchInDirectory(void *buffer, uint32_t n, char target[11]);

void *fat12_cd(void *buffer, uint32_t n, char target[11], partition_t *partition, uint32_t *new_n);

void fat12_mkdir(partition_t *partition, void *cwd, uint32_t n, uint32_t parent_cluster, const char *name);

void fat12_touch(partition_t *partition, void *cwd, uint32_t size, uint32_t n, uint32_t parent_cluster, const char *name, void *buffer);

void fat12_remove(partition_t *partition, void *cwd, uint32_t n, uint32_t parent_cluster, const char *name);

void fat12_read(partition_t *partition, void *cwd, const char *name, void *buffer, uint32_t n, uint32_t dir_n);

#endif
