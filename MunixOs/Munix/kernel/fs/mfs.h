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


#define MAX_NAME_LEN 19

/*
 * ahora, algo importante, los atributos
 */

#define MFS_ATTR_TYPE_MASK 00000111b
#define MFS_ATTR_TYPE_EMPTY 00000000b
#define MFS_ATTR_TYPE_FILE 00000001b
#define MFS_ATTR_TYPE_DIR 00000010b
#define MFS_ATTR_TYPE_SYMLINK 00000011b
#define MFS_ATTR_TYPE_DEVICE 00000100b
#define MFS_ATTR_TYPE_FIFO 00000101b
#define MFS_ATTR_TYPE_SOCKET 00000110b
#define MFS_ATTR_TYPE_HARDLINK 00000111b

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

typedef struct {
  uint32_t num_entries;
  uint32_t dirBlocks;
  uint16_t permissions;
  uint16_t owner_id;
  uint16_t group_id;
  char owner_name[17 + 1]; // máximo para el user, es una herramienta para la legibilidad sobre todo
} __attribute__((packed)) mfs_dir_header_t;

void MBRformatMFS(disk_t *disk, partition_manager_t *p_mng, uint8_t n, uint8_t blockSize, lba_t start, uint32_t size);

void loadMFSuperBlock(partition_t *partition, void *buffer);
void saveMFSuperBlock(partition_t *partition, void *buffer);
void makeMFSroot(partition_t *partition, mfs_superblock_t *block, void *ifat_table, uint32_t size);

#endif
