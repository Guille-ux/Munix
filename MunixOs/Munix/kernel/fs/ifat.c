#include "ifat.h"
#include "../include/memory.h"
#include "../include/libcs2.h"

void IFATload(mfs_superblock_t *sblock, void *buffer, partition_t *partition) {
	readPartition(partition, buffer, sblock->IFATStart, sblock->IFATSize);
}

void IFATSave(mfs_superblock_t *sblock, void *buffer, partition_t *partition) {
	writePartition(partition, buffer, sblock->IFATStart, sblock->IFATSize);
}

uint32_t IFATreadEntry(mfs_superblock_t *sblock, void *table, uint32_t index) {
	if (index==IFAT_END_OF_CHAIN ||
	    index==IFAT_BAD_BLOCK ||
	    index==IFAT_TOMBSTONE ||
	    index==IFAT_FREE_BLOCK ||
	    sblock==NULL ||
	    table==NULL ||
	    index >= sblock->NumBlocks
	    ) {
		return IFAT_BAD_BLOCK;
	} // para que de error, básicamente
	uint32_t *t = (uint32_t*)table;
	return t[index];
}

void IFATwriteEntry(mfs_superblock_t *sblock, void *table, uint32_t index, uint32_t val) {
	if (index==IFAT_END_OF_CHAIN ||
	    index==IFAT_BAD_BLOCK ||
	    index==IFAT_TOMBSTONE ||
	    index==IFAT_FREE_BLOCK ||
	    sblock==NULL ||
	    table==NULL ||
	    index >= sblock->NumBlocks
	    ) {
		return IFAT_BAD_BLOCK;
	} // para que de error, básicamente
	uint32_t *t = (uint32_t*)table;
	t[index]=val;
}
