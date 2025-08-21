#ifndef MFS
#define MFS

#include <stdint.h>
#include <stddef.h>
#include "../disk/general.h"
#include "../partitions/partitions.h"
#include "../disk/diski.h"
#include "../partitions/partition_mng.h"

#define MFS_SIGNATURE 0x1FABCDF0

typedef struct {
  char FSname[8]; // nombre del FS pensado para ser legible, no para identificarlo
  uint32_t version;
  uint32_t reserved; // nº de sectores de 512 bytes que estan reservados
  uint32_t SectorsPerBlock; // nº de sectores por bloque
  uint32_t RootBlock; // el indice del bloque del directorio raíz
  uint32_t RootSize; // tamaño de la raíz en sectores
  lba_t IFATStart; // donde empieza la tabla en lba
  uint32_t IFATSize; // tamaño de la tabla de asignación
  uint32_t IFATFlags; // flags del ifat, definidas más adelante
  lba_t DataBegin; // offset en el que empiezan los datos, en LBA
  uint32_t DataSize; // número de sectores de datos
  
  uint32_t IFATchecksumType; // id para el algoritmo utilizado
  uint32_t IFATchecksum; // el checksum, el hash, solo 32 bits, si, una pena, pero es lo que hay
  
  uint32_t NumBlocks; // cuantos bloques tenemos
  uint32_t FreeBlocks; // bloques libres actualmente
  uint32_t LastAllocated; // último bloque asignado
  
  uint8_t fs_status;
  
  char volName[8]; // nombre para identificar la partición, es opcional
  char whoFormat[8]; // un identificador para saber quien lo formateo
  uint32_t signature; // una firma común para identificar esto 0x1FABCDF0
  uint8_t padding[417]; // para que quepa en un bloque de 512 bytes perfectamente
} __attribute__((packed)) mfs_superblock_t;


#define MAX_NAME_LEN 18

/*
 * ahora, algo importante, los atributos
 */

#define MFS_ATTR_TYPE_MASK 0x07
#define MFS_ATTR_TYPE_EMPTY 0x00
#define MFS_ATTR_TYPE_FILE 0x01
#define MFS_ATTR_TYPE_DIR 0x02
#define MFS_ATTR_TYPE_SYMLINK 0x03
#define MFS_ATTR_TYPE_DEVICE 0x04
#define MFS_ATTR_TYPE_FIFO 0x05
#define MFS_ATTR_TYPE_SOCKET 0x06
#define MFS_ATTR_TYPE_HARDLINK 0x07

#define MFS_ATTR_HIDDEN (1 << 3) // oculto
#define MFS_ATTR_IMMUTABLE (1 << 4) // inmutable, menos por el sistema
#define MFS_ATTR_TMP (1 << 5) // temporal
#define MFS_ATTR_RO (1 << 6) // read only
#define MFS_ATTR_EXECUTABLE (1 << 7)// es ejecutable??


typedef struct {
  char name[MAX_NAME_LEN + 1]; // longitud máxima más null terminator
  uint8_t attr; // atributos
  uint32_t modified; // timestamp, (mes-día/hora-minuto)
  uint32_t blockSize; // tamaño en bloques, 32 bits no pueden almacenar si es de más de 4 gigas
  uint32_t first_block; // primer bloque
} __attribute__((packed)) mfs_entry_t;

#define MAX_OWNER_NAME 13

typedef struct {
  uint32_t num_entries;
  uint32_t dirBlocks;
  uint16_t permissions;
  uint16_t owner_id;
  uint16_t group_id;
  uint32_t block;
  char owner_name[MAX_OWNER_NAME + 1]; // máximo para el user, es una herramienta para la legibilidad sobre todo
} __attribute__((packed)) mfs_dir_header_t;

void MBRformatMFS(disk_t *disk, uint8_t n, uint32_t blockSize, lba_t start, uint32_t size);

void loadMFSuperBlock(partition_t *partition, void *buffer);
void saveMFSuperBlock(partition_t *partition, void *buffer);
void makeMFSroot(partition_t *partition, mfs_superblock_t *block, void *ifat_table, uint32_t size);


void MFSloaDir(mfs_superblock_t *block, partition_t *partition, void *table, void *buffer, uint32_t n, uint32_t index);
void MFSsaveDir(mfs_superblock_t *block, partition_t *partition, void *table, void *buffer, uint32_t n, uint32_t index);

mfs_entry_t *MFSearchEntry(void *dir, const char *name);
int MFScreateEntry(mfs_superblock_t *block, void *dir, void *table, const char *name, uint8_t attr, uint32_t blockSize, partition_t *partition);

int mfsMkDir(mfs_superblock_t *block, void *dir, const char *name, partition_t *partition, void *table, uint32_t size);

int MFSchdir(void **dir, const char *name, mfs_superblock_t *block, partition_t *partition, void *table);

static inline mfs_dir_header_t *MFStatDir(void *dir) {
	return (mfs_dir_header_t*)dir;
}
void MFStatChDir(void *dir, uint16_t permissions, uint16_t owner_id, uint16_t group_id, const char *owner_name);

#endif
