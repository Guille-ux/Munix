#include "fs.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include "mfs.h"
#include "ifat.h"

#define MFS_DIR_BLOCKS 1

#define kmallloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

explorer_t *mfs_init_explorer(partition_t *partition, explorer_t *explorer, uint16_t owner_id, uint16_t group_id) {
	if (partition==NULL || explorer == NULL) {
		kprintf("[EXPLORER]: Invalid Parameters!\n");
		return NULL;
	}

	if (partition->fs_type!=FS_TYPE_MFS) {
		kprintf("[EXPLORER]: Partition isn't MFS!\n");
		return NULL;
	}

	mfs_superblock_t *block = (mfs_superblock_t*)kmalloc(sizeof(mfs_superblock_t));
	loadMFSuperBlock(partition, block);
	if (block->signature!=MFS_SIGNATURE) {

		kprintf("[EXPLORER]: Invalid MFS Partition!\n");
		kfree(block);
		return NULL;
	} else {
		kprintf("[EXPLORER]: Valid MFS Partition\n");
	}

	
	void *table = kmalloc(block->IFATSize);
	
	IFATload(block, table, partition);
	
	mfs_meta_t *metadata = (mfs_meta_t*)kmalloc(sizeof(mfs_meta_t));
	metadata->superblock = block;
	metadata->ifat_table = table;
	explorer->meta = metadata;
	memcpy(explorer->path, "/", 1);
	explorer->path[1]='\0';
	explorer->group_id = group_id;
	explorer->owner_id = owner_id;
	if (block->RootBlock == 0) {
		makeMFSroot(partition, block, table, MFS_DIR_BLOCKS); // usaremos por defecto 1
		block->RootSize=MFS_DIR_BLOCKS;
		IFATSave(block, table, partition);
		saveMFSuperBlock(partition, block);
	}
	uint32_t n = block->RootSize*block->SectorsPerBlock*512;
	explorer->cwd = kmalloc(n);
	MFSloaDir(block, partition, table, explorer->cwd, n, block->RootBlock);
/*
 */
}
