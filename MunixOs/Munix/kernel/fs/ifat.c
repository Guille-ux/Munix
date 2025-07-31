#include "ifat.h"
#include "../include/memory.h"
#include "../include/libcs2.h"

void IFATload(mfs_superblock_t *sblock, void *buffer, partition_t *partition) {
	readPartition(partition, buffer, sblock->IFATStart, sblock->IFATSize);
}

void IFATSave(mfs_superblock_t *sblock, void *buffer, partition_t *partition) {
	writePartition(partition, buffer, sblock->IFATStart, sblock->IFATSize);
}


