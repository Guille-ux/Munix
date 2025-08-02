#ifndef IFAT
#define IFAT

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../disk/general.h"
#include "../partitions/partitions.h"
#include "../partitions/partition_mng.h"
#include "../disk/diski.h"
#include "mfs.h"

#define IFAT_BAD_BLOCK 0xFFFFFFFE // no deberiamos asignar bloques que no podemos usar
#define IFAT_TOMBSTONE 0xFFFFFFFD // una tombstone
#define IFAT_FREE_BLOCK 0x00000000 // un bloque libre
#define IFAT_END_OF_CHAIN 0xFFFFFFFF // el final de la cadena

void IFATload(mfs_superblock_t *sblock, void *buffer, partition_t *partition);
void IFATSave(mfs_superblock_t *sblock, void *buffer, partition_t *partition);uint32_t IFATreadEntry(mfs_superblock_t *sblock, void *table, uint32_t index);
void IFATwriteEntry(mfs_superblock_t *sblock, void *table, uint32_t index, uint32_t val);


void IFATreadChain(mfs_superblock_t *sblock, void *table, uint32_t index, void *buffer, partition_t *partition, uint32_t max);

void IFATremoveChain(mfs_superblock_t *sblock, void *table, uint32_t index, bool clean);

void IFATcleanTombstones(mfs_superblock_t *sblock, void *table);

void IFATwriteChain(mfs_superblock_t *sblock, void *table, uint32_t index, void *buffer, partition_t *partition, uint32_t max);

void IFATallocChain(mfs_superblock_t *sblock, void *table, uint32_t n, uint32_t *start);

#endif
