#include "fs.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../include/handler.h"
#include "mfs.h"
#include "ifat.h"
#include "fsd.h"

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

	explorer->ls = mfs_ls;
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
	explorer->destroy = mfs_destroy;
	explorer->new_fd = mfs_new_fd;


	mfs_superblock_t *block = (mfs_superblock_t*)kmalloc(sizeof(mfs_superblock_t));
	loadMFSuperBlock(partition, block);
	if (block->signature!=MFS_SIGNATURE) {

		kprintf("[EXPLORER]: Invalid MFS Partition!\n");
		kfree(block);
		return NULL;
	}

	kprintf("[EXPLORER]: Valid MFS Partition\n");
	
	void *table = kmalloc(block->IFATSize*512);
	
	IFATload(block, table, partition);
	
	mfs_meta_t *metadata = (mfs_meta_t*)kmalloc(sizeof(mfs_meta_t));
	metadata->superblock = block;
	metadata->ifat_table = table;
	explorer->meta = metadata;
	explorer->path[0]='/';
	explorer->path[1]='\0';
	explorer->group_id = group_id;
	explorer->owner_id = owner_id;
	
	if (block->RootBlock == 0) {
		kprintf("Creating Root...\n");
		makeMFSroot(partition, block, table, MFS_DIR_BLOCKS); // usaremos por defecto 1
		block->RootSize=MFS_DIR_BLOCKS;
		IFATSave(block, table, partition);
		saveMFSuperBlock(partition, block);
	}
	uint32_t n = block->RootSize*block->SectorsPerBlock*512;
	explorer->cwd = alloc_handle();
	*explorer->cwd = kmalloc(n);
	MFSloaDir(block, partition, table, *explorer->cwd, n, block->RootBlock);
	return explorer;
}

int mfs_ls(explorer_t *explorer, files_t *list) {
	if (explorer == NULL || list == NULL) return -1;

	list->current_idx = 0;
	list->fs = explorer;
	list->next = mfs_ls_next;
	list->count = mfs_ls_count;

	return 0;
}

static inline mfs_entry_t *mfs_ls_get_entries(files_t *iterator) {
	return (mfs_entry_t*)(((size_t)(*iterator->fs->cwd)) + sizeof(mfs_dir_header_t));
}

int mfs_ls_next(files_t *iterator, file_item_t *item) {
	if (iterator == NULL || item == NULL) return -1;
	if (!(iterator->current_idx < ((mfs_dir_header_t*)(*iterator->fs->cwd))->num_entries)) return -1;

	mfs_entry_t *entries = mfs_ls_get_entries(iterator);
	if (entries[iterator->current_idx].first_block == IFAT_FREE_BLOCK) {
		memcpy(item->name, "NULL", 5);
		return -1;
	}
	while (entries[iterator->current_idx].first_block == IFAT_TOMBSTONE) {
		iterator->current_idx++;
	}
	mfs_entry_t entry = entries[iterator->current_idx++];
	memcpy(item->name, entry.name, strlen(entry.name)+1);

	return 0;
}

uint32_t mfs_ls_count(files_t *iterator) {
	if (iterator == NULL) return 0;
	
	uint32_t num_files = 0;
	mfs_entry_t *entries = mfs_ls_get_entries(iterator);

	for (uint32_t i = 0; i<((mfs_dir_header_t*)(*iterator->fs->cwd))->num_entries; i++) {
		if (entries[i].first_block == IFAT_TOMBSTONE) continue;
		if (entries[i].first_block == IFAT_FREE_BLOCK) break;
		num_files++;
	}


	return num_files;
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

int mfs_destroy(explorer_t *explorer) {
	free_handle(explorer->cwd);
	kfree(*explorer->cwd);
	mfs_meta_t *metadata = (mfs_meta_t*)explorer->meta;
	kfree(metadata->superblock);
	kfree(metadata->ifat_table);
	kfree(metadata);
	return 0;
}

int mfs_new_fd(explorer_t *explorer, file_t *fd, const char *name) {
	mfs_entry_t *entry = MFSearchEntry(*explorer->cwd, name);
	// metadatos no específicos (el nombre no cuenta)
	fd->explorer = explorer;
	// metiendo metadatos específicos
	fd->as.mfs.first_block = entry->first_block;
	fd->as.mfs.size_in_blocks = entry->blockSize;
	fd->as.mfs.modified = entry->modified;
	fd->as.mfs.attr = entry->attr;
	fd->as.mfs.folder_block = ((mfs_dir_header_t*)(*explorer->cwd))->block;
	fd->as.mfs.folder_size = ((mfs_dir_header_t*)(*explorer->cwd))->dirBlocks;
	// pegando nombre
	strcpy(fd->name, name);
	// metiendo las funciones
	fd->read = mfs_fd_read;
	fd->write = mfs_fd_write;
	fd->extend = mfs_fd_extend;
	return 0;
}

int mfs_fd_read(file_t *file, void *buffer, size_t size) {
	void *table = ((mfs_meta_t*)file->explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)file->explorer->meta)->superblock;

	IFATreadChain(block, table, file->as.mfs.first_block, buffer, file->explorer->partition, size);
	return 0;
}

int mfs_fd_write(file_t *file,  void *buffer, size_t size) {
	void *table = ((mfs_meta_t*)file->explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)file->explorer->meta)->superblock;

	IFATwriteChain(block, table, file->as.mfs.first_block, buffer, file->explorer->partition, size);
	return 0;
}

int mfs_fd_extend(file_t *file) {
	void *table = ((mfs_meta_t*)file->explorer->meta)->ifat_table;
	mfs_superblock_t *block = ((mfs_meta_t*)file->explorer->meta)->superblock;	
	uint32_t new_end = 0; // abierto para optimizaciones
	IFATallocateChain(block, table, 1, &new_end); // esto asigna un bloque nuevo
	uint32_t file_start = file->as.mfs.first_block;
	
	IFATappendChain(block, table, file_start, new_end, file->explorer->partition);

	file->as.mfs.size_in_blocks++;

	// ahora toca algo complejo, modificar la carpeta, ouch esto dolera
	uint32_t dir_size = file->as.mfs.folder_size * block->SectorsPerBlock;

	void *dir = malloc(file->as.mfs.folder_size * block->SectorsPerBlock);
	
	IFATreadChain(block, table, file->as.mfs.folder_block, dir, file->explorer->partition, file->as.mfs.folder_size * block->SectorsPerBlock);

	mfs_entry_t *usable = (mfs_entry_t*)dir;

	while (((uint32_t)++usable) < dir_size) {
		if (strcmp(usable->name, file->name)==0) {
			// lo encontramos aqui hay q escribir
			// ns, supongo q diremos q alargamos el archivo
			usable->blockSize++;
			break;
		}
	}

	IFATwriteChain(block, table, file->as.mfs.folder_block, dir, file->explorer->partition, file->as.mfs.folder_size * block->SectorsPerBlock);

	free(dir);

	return 0;
}

