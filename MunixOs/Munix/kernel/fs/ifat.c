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
	return t[index-1]; // porque el 0 no puede ser, entonces restamos 1, para no desperdiciarlo
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
		return;
	} // para que de error, básicamente
	uint32_t *t = (uint32_t*)table;
	t[index-1]=val;
}

void IFATreadChain(mfs_superblock_t *sblock, void *table, uint32_t index, void *buffer, partition_t *partition, uint32_t max) {
	if (buffer==NULL || table==NULL || sblock==NULL) return;
	lba_t lba=sblock->DataBegin;

	uint32_t start_idx=index;
	uint32_t current_idx=index;
	uint32_t new_idx;
	bool should_break=false;
	uint32_t read=0;
	while (read < (max/512)/sblock->SectorsPerBlock) {
		do {
			new_idx = IFATreadEntry(sblock, table, current_idx);
			if (new_idx == IFAT_BAD_BLOCK ||
			    new_idx == IFAT_FREE_BLOCK ||
			    new_idx == IFAT_TOMBSTONE) {
				
				return;
			}

			if (new_idx == IFAT_END_OF_CHAIN) {
				should_break = true;
				break;
			}
		
			current_idx++;
		} while (new_idx == current_idx);
		
		readPartition(partition, buffer, lba, sblock->SectorsPerBlock*(current_idx-start_idx-1));

		buffer+=sblock->SectorsPerBlock*512*(current_idx-start_idx-1);

		uint64_t tmp = lba2uint64(lba);
		tmp += sblock->SectorsPerBlock*(current_idx-start_idx-1);
		lba = uint64_2_lba(tmp);

		start_idx = new_idx;
		read += sblock->SectorsPerBlock*(current_idx-start_idx-1);
		if (should_break==true) break;
	}
}

void IFATremoveChain(mfs_superblock_t *sblock, void *table, uint32_t index, bool clean) {
	uint32_t current_idx=0;
	uint32_t next_idx;
	uint32_t fill_val = (clean) ? IFAT_FREE_BLOCK : IFAT_TOMBSTONE;
	do  {
		next_idx = IFATreadEntry(sblock, table, current_idx);
		IFATwriteEntry(sblock, table, current_idx, fill_val);
		current_idx = next_idx;
	}
	while  (next_idx!= IFAT_BAD_BLOCK ||
		next_idx!= IFAT_END_OF_CHAIN ||
		next_idx!= IFAT_FREE_BLOCK ||
		next_idx!= IFAT_TOMBSTONE);
}

void IFATcleanTombstones(mfs_superblock_t *sblock, void *table) {
	uint32_t current=1;
	uint32_t block;
	uint32_t freed=0;
	while (current < sblock->NumBlocks) {
		block = IFATreadEntry(sblock, table, current);
		if (block == IFAT_TOMBSTONE) {
			IFATwriteEntry(sblock, table, current, IFAT_FREE_BLOCK);
			freed++;
		}
		current++;
	}
	sblock->FreeBlocks += freed;
}

void IFATwriteChain(mfs_superblock_t *sblock, void *table, uint32_t index, void *buffer, partition_t *partition, uint32_t max) {
	if (buffer==NULL || table==NULL || sblock==NULL) return;
	lba_t lba=sblock->DataBegin;

	uint32_t start_idx=index;
	uint32_t current_idx=index;
	uint32_t new_idx;
	bool should_break=false;
	uint32_t write=0;
	while (write < (max/512)/sblock->SectorsPerBlock) {
		do {
			new_idx = IFATreadEntry(sblock, table, current_idx);
			if (new_idx == IFAT_BAD_BLOCK ||
			    new_idx == IFAT_FREE_BLOCK ||
			    new_idx == IFAT_TOMBSTONE) {
				
				return;
			}

			if (new_idx == IFAT_END_OF_CHAIN) {
				should_break = true;
				break;
			}
		
			current_idx++;
		} while (new_idx == current_idx);
		
		writePartition(partition, buffer, lba, sblock->SectorsPerBlock*(current_idx-start_idx-1));

		buffer+=sblock->SectorsPerBlock*512*(current_idx-start_idx-1);

		uint64_t tmp = lba2uint64(lba);
		tmp += sblock->SectorsPerBlock*(current_idx-start_idx-1);
		lba = uint64_2_lba(tmp);

		start_idx = new_idx;
		write += sblock->SectorsPerBlock*(current_idx-start_idx-1);
		if (should_break==true) break;
	}
}

void IFATallocChain(mfs_superblock_t *sblock, void *table, uint32_t n, uint32_t *start) {
	*start = 0;
	uint32_t last;
	uint32_t current=0;
	uint32_t count=0;
	while (current++ < sblock->NumBlocks && count < n) {
		if (IFATreadEntry(sblock, table, current)!=IFAT_FREE_BLOCK) continue;
		if (*start == 0) {
			*start = current;
			last = current;
		} else {
			IFATwriteEntry(sblock, table, last, current);
			last = current;
		}

		count++;
	}
	IFATwriteEntry(sblock, table, last, IFAT_END_OF_CHAIN);
	if (count < n) {
		IFATremoveChain(sblock, table, *start, true);
		*start = 0;
		return;
	}
	sblock->FreeBlocks -= n;
}
