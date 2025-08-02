#include "mfs.h"
#include "../partitions/mbr.h"
#include "../partitions/partitions.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include "ifat.h"

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

void MBRformatMFS(disk_t *disk, partition_manager_t *p_mng, uint8_t n, uint8_t blockSize, lba_t start, uint32_t size) {
	if (n > 3) return;
	if (blockSize % 512 != 0) return; 
	mbr_t *mbr = (mbr_t*)kmalloc(sizeof(mbr_t));
	lba_t lba = {.lo=0, .hi=0};
	disk->read(disk, mbr, lba, 1);
	mbr_partition_t *p = &mbr->data.partitions[n];
	p->lba_start=start.lo;
	p->size=size;
	p->type=FS_TYPE_MFS;
	formatMBRreload(disk, mbr, p_mng);
	kfree(mbr);
	partition_t *partition = NULL;
	for (int i=0;i<p_mng->partitions_count;i++) {
		if (p_mng->partitions[i].parent_disk!=disk) continue;
		if (p_mng->partitions[i].lba_start.lo==start.lo) {
			partition = &p_mng->partitions[i];
			break;
		}
	}
	if (partition==NULL)  {
		kprintf("[MFS Maker]: Partition not found\n");
		return;
	}
	mfs_superblock_t *mfs_spr=(mfs_superblock_t*)kmalloc(sizeof(mfs_superblock_t));
	memcpy(mfs_spr->FSname, "MunixFS", 8);
	memcpy(mfs_spr->whoFormat, "MunixOS", 8);
	memcpy(mfs_spr->volName, "MFS VOL", 8);
	uint32_t base = 128;
	uint32_t remaining = size - 1;
	uint32_t n_groups = remaining / ((128*blockSize)+1);
	uint32_t ifat_size = n_groups;
	uint32_t data_size = n_groups * base;
	mfs_spr->version=1;
	mfs_spr->reserved=1; // reservamos el superbloque
	mfs_spr->SectorsPerBlock=blockSize;
	mfs_spr->RootBlock=0;
	mfs_spr->IFATStart=(lba_t){.lo=1, .hi=0};
	mfs_spr->IFATSize=ifat_size;
	mfs_spr->IFATFlags=0;
	mfs_spr->IFATchecksumType=0; // 0 indica que no hay checksum
	mfs_spr->IFATchecksum=0;
	mfs_spr->LastAllocated=0;
	mfs_spr->DataSize=data_size;
	mfs_spr->DataBegin = (lba_t){.lo=1+ifat_size, .hi=0};
	mfs_spr->FreeBlocks=data_size;
	mfs_spr->NumBlocks=data_size;
	mfs_spr->fs_status=0;
	mfs_spr->signature=MFS_SIGNATURE;
	writePartition(partition, mfs_spr, lba, 1);
	uint8_t *buffer = (uint8_t*)mfs_spr;
	memset(buffer, 0, 512);
	lba.lo=1;
	for (;lba.lo<ifat_size;lba.lo++) {
		writePartition(partition, buffer, lba, 1);
	}
	kfree(buffer);	
}

void loadMFSuperBlock(partition_t *partition, void *buffer) {
	readPartition(partition, buffer, uint64_2_lba(0), 1);
}

void saveMFSuperBlock(partition_t *partition, void *buffer) {
	writePartition(partition, buffer, uint64_2_lba(0), 1);
}

static inline void mfsDirHeaders(void *dir, uint32_t n_entries, uint32_t blocks, uint16_t permissions, uint16_t owner_id, uint16_t group_id, const char *name, uint32_t block) {
	mfs_dir_header_t *header=dir;

	header->num_entries=n_entries;
	header->dirBlocks=blocks;
	header->permissions=permissions;
	header->owner_id=owner_id;
	header->group_id=group_id;
	memcpy(header->owner_name, name, MAX_OWNER_NAME);
	header->owner_name[MAX_OWNER_NAME]='\0';
	header->block=block;
}
static void mfsNewDirEntry(const char *name, uint8_t attr, uint32_t modified, uint32_t blockSize, uint32_t first_block, void *dir) {
	mfs_entry_t *entries = (void*)(dir+32);
	mfs_entry_t *entry=NULL;
	for (int i=0;i<((mfs_dir_header_t*)dir)->num_entries;i++) {
		if (entries[i].first_block==IFAT_TOMBSTONE) {
			entry=&entries[i];
			break;
		} else if (entries[i].first_block==IFAT_FREE_BLOCK) {
			entry=&entries[i];
			break;
		}
	}
	if (entry==NULL) return;
	memcpy(entry->name, name, MAX_NAME_LEN);
	entry->name[MAX_NAME_LEN]='\0';
	entry->attr=attr;
	entry->modified=modified;
	entry->blockSize=blockSize;
	entry->first_block=first_block;
}

void makeMFSroot(partition_t *partition, mfs_superblock_t *block, void *ifat_table, uint32_t size) {
	uint32_t root_start=0;
	IFATallocChain(block, ifat_table, size, &root_start);
	block->RootBlock = root_start;
	uint32_t max = size*block->SectorsPerBlock*512;
	void *dir = kmalloc(max);
	IFATreadChain(block, ifat_table, root_start, dir, partition, max);
	memset(dir, 0, max);
	uint8_t attr = MFS_ATTR_IMMUTABLE | MFS_ATTR_TYPE_DIR;
	uint32_t modified=0; // de momento no tengo forma de hacer timestamps
	mfsDirHeaders(dir, size*512*block->SectorsPerBlock/32, size, 0, 0, 0, "MunixOs", root_start);
	mfsNewDirEntry(".", attr, modified, size, root_start, dir);
	IFATwriteChain(block, ifat_table, root_start, dir, partition, max);
}

void MFSloaDir(mfs_superblock_t *block, partition_t *partition, void *table, void *buffer, uint32_t n, uint32_t index) {
	IFATreadChain(block, table, index, buffer, partition, n);
}

void MFSsaveDir(mfs_superblock_t *block, partition_t *partition, void *table, void *buffer, uint32_t n, uint32_t index) {
	IFATwriteChain(block, table, index, buffer, partition, n);
}

mfs_entry_t *MFSearchEntry(void *dir, const char *name) {
	mfs_dir_header_t *header = dir;
	mfs_entry_t *entries = (void*)(dir+32);
	mfs_entry_t *entry=NULL;
	for (int i=0;i<header->num_entries;i++) {
		if (strcmp(name, entries[i].name)==0) {
			entry=&entries[i];
			break;
		}
	}
	return entry;
}
void MFScreateEntry(mfs_superblock_t *block, void *dir, void *table, const char *name, uint8_t attr, uint32_t blockSize, partition_t *partition) {
	mfs_dir_header_t *header = dir;
	mfs_entry_t *nothing = MFSearchEntry(dir, name);
	if (nothing != NULL) return;

	uint32_t start=0;
	IFATallocChain(block, table, blockSize, &start);
	if (start==0) return; // bro, no se asigno nada, hubo un error
	

	uint32_t modified=0; // en el futuro sera algun timestamp
	mfsNewDirEntry(name, attr, modified, blockSize, start, dir);

	IFATwriteChain(block, table, header->block, dir, partition, header->dirBlocks*512*block->SectorsPerBlock);
}
