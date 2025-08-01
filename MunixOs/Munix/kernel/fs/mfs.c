#include "mfs.h"
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
	for (int i=0;i<p_mng->partition_count;i++) {
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
