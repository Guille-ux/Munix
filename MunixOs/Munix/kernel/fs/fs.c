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
	explorer->cd = mfs_cd;
	explorer->mkdir = mfs_mkdir;
	explorer->mod = mfs_mod;
	explorer->chmod = mfs_chmod;
	explorer->stat = mfs_stat;
	explorer->chstat = mfs_chstat;
	explorer->size = mfs_size;
	explorer->time = mfs_time;
	explorer->chtime = mfs_chtime;
	explorer->touch = mfs_touch;
	explorer->cat = mfs_cat;
	explorer->write = mfs_write;
	explorer->remove = mfs_remove;
	explorer->clean = mfs_clean;
}

int mfs_cd(explorer_t *explorer, const char *dir_name) {
	mfs_superblock_t *block=((mfs_meta_t*)explorer->meta)->superblock;
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table; 
	if (strcmp(dir_name, ".")==0) {
		return 0;
	}

	int ret = MFSchdir(explorer->cwd, dir_name, block, explorer->partition, table);

	if (ret != 0) return ret;

	if (strcmp(dir_name, "..")==0) {
		if (strcmp(explorer->path, "/")==0) {
			return 0; // ya estamos en la raíz
		}

		char *ch = strrchr(explorer->path, '/');
		if (ch != NULL) {
			*ch = '\0';
		}
		if (explorer->path[0]=='\0') {
			explorer->path[0]='/';
			explorer->path[1]='\0';
		}
	} else if (strcmp(explorer->path, "/")==0) {
		strcat(explorer->path, dir_name);
	} else if ((1 + strlen(explorer->path) + strlen(dir_name)) < 256) {
		strcat(explorer->path, "/");
		strcat(explorer->path, dir_name);
	}

	return ret;
}

int mfs_mkdir(explorer_t *explorer, const char *name) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	int ret = mfsMkDir(block, *explorer->cwd, name, explorer->partition, table, MFS_DIR_BLOCKS);
	if (ret == 0) {
		saveMFSuperBlock(explorer->partition, block);
		IFATSave(block, table, explorer->partition);
	}
	return ret;
}

int mfs_mod(explorer_t *explorer, uint16_t *permissions, uint16_t *group, uint16_t *owner) {
	mfs_dir_header_t *tmp = MFStatDir(*explorer->cwd);
	*permissions = tmp->permissions;
	*group = tmp->group_id;
	*owner = tmp->owner_id;
	return 0;
}

static inline void mfs_sadir(explorer_t *explorer, mfs_superblock_t *block, void *table) {	
	mfs_dir_header_t *tmp = MFStatDir(*explorer->cwd);
	MFSsaveDir(block, explorer->partition, table, *explorer->cwd, tmp->dirBlocks*512*block->SectorsPerBlock, tmp->block);
}

int mfs_chmod(explorer_t *explorer, uint16_t permissions, uint16_t group, uint16_t owner) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	MFStatChDir(*explorer->cwd, permissions, owner, group, "idk");
	
	mfs_sadir(explorer, block, table);

	return 0;
}

int mfs_stat(explorer_t *explorer, const char *name, uint8_t *attr) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	*attr = entry->attr;
	return 0;
}

int mfs_chstat(explorer_t *explorer, const char *name, uint8_t attr) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	entry->attr = attr;
	
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	mfs_sadir(explorer, block, table);	

	return 0;
}

int mfs_size(explorer_t *explorer, const char *name, uint32_t *size) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1; // si no existe devolver error
	
	*size = entry->blockSize;

	return 0;
}

int mfs_time(explorer_t *explorer, const char *name, uint32_t *mod) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	*mod = entry->modified;

	return 0;
}

int mfs_chtime(explorer_t *explorer, const char *name, uint32_t mod) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	entry->modified = mod;

	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;

	mfs_sadir(explorer, block, table);

	return 0;	
}

int mfs_touch(explorer_t *explorer, const char *name, uint32_t size) {
	uint8_t attr = MFS_ATTR_TYPE_FILE;
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;

	int ret = MFScreateEntry(block, *explorer->cwd, table, name, attr, size, explorer->partition);
	
	if (ret != 0) return ret;

	mfs_sadir(explorer, block, table);
	IFATSave(block, table, explorer->partition);

	return ret;
}

int mfs_cat(explorer_t *explorer, const char *name, void *buffer, uint32_t amount) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	IFATreadChain(block, table, entry->first_block, buffer, explorer->partition, amount);

	return 0;
}

int mfs_write(explorer_t *explorer, const char *name, void *content, uint32_t n) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	IFATwriteChain(block, table, entry->first_block, content, explorer->partition, n);

	return 0;
}

int mfs_remove(explorer_t *explorer, const char *name) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	if (entry==NULL) return -1;

	IFATremoveChain(block, table, entry->first_block, false);

	mfs_sadir(explorer, block, table);
	IFATSave(block, table, explorer->partition);

	return 0;
}

int mfs_clean(explorer_t *explorer) {
	void *table = ((mfs_meta_t*)explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)explorer->meta)->superblock;

	IFATcleanTombstones(block, table);
	IFATSave(block, table, explorer->partition);
	saveMFSuperBlock(explorer->partition, block);

	return 0;
}

