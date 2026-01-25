#include "tar.h"
#include "../include/libcs2.h"

static size_t roundToBlocks(size_t n) {	
	return (n + TAR_BLOCK_SIZE - 1) & ~(TAR_BLOCK_SIZE-1);
}

size_t oct2int(char *chain, char length) {
	size_t size = 0;
	
	for (int i=0;i < length-1;i++) {
		size += chain[i] - '0';
		size *= 8;
	}
	size /= 8;

	return size;
}


void int2oct(char *chain, char length, size_t n) {
	size_t size = 0;
	chain[length-1] = '\0';
	
	for (int i=length-1;i > 0;i--) {
		chain[i] = n % 8 + '0';
		n /= 8;
	}
}

tar_t *scanTarFile(tar_t *tar, void *block) {
	tar_entry_t *entries = (tar_entry_t*)malloc(512);
	size_t cap=512;
	size_t in_use = 0;
	tar->n_entries = 0;

	tar->start = block;
	size_t p = 0;
	while (1) {
		tar_header_t *header = &block[p];
		if (block[p]=='\0') {
			break; // genial, llegamos al final del archivo
		}
		tar->n_entries++;
		in_use+=sizeof(tar_entry_t);
		if (in_use >= cap) {
			cap *= 2;
			void *newTable = malloc(cap);
			if (newTable == NULL) {
				return NULL; // error?
			}
			memcpy(newTable, entries, in_use);
			free(entries);
			entries = newTable;
		}
		entries[n_entries-1].size = oct2int(header->size, 12);
		entries[n_entries-1].uid = oct2int(header->uid, 8);
		entries[n_entries-1].gid = oct2int(header->gid, 8);
		p += roundToBlocks(entries[n_entries-1].size) + TAR_BLOCK_SIZE;
	}
	tar->next_addr = (void*)p;
	tar->entries = entries;

	return tar;
}

ntar_t *initTarFile(ntar_t *tar) {
	tar->base = malloc(TAR_START_SIZE);
	tar->cap = TAR_START_SIZE;
	tar->size = 0;
	tar->n_entries = 0;

	return tar;
}

ntar_t *appendTarFile(ntar_t *tar, void *block, size_t uid, size_t gid, size_t mtime, size_t size, size_t mode, const char *name) {
	if (tar->size + size + TAR_BLOCK_SIZE >= tar->cap) {
		tar->cap = tar->cap * 2 + (tar->size + size + 512 - tar->cap * 2);
		void *newBase = malloc(tar->cap);
		memcpy(newBase, tar->base, tar->size);
		free(tar->base); 
		tar->base = newBase;
	}
	tar->n_entries++;
	// añadimos la cabecera
	tar_header_t *header = ((size_t)tar->base+tar->size);
	memcpy(header->name, name, 100);
	int2oct(header->mode, 8, mode);
	int2oct(header->uid, 8, uid);
	int2oct(header->gid, 8, gid);
	int2oct(header->size, 12, size);
	int2oct(header->mtime, 12, mtime);


	tar->size += TAR_BLOCK_SIZE;
	memcpy((void*)((size_t)tar->base+tar->size), block, size);
	tar->size += roundToBlocks(size);
	return tar;
}

void endTarFile(ntar_t *tar) {
	if (tar->size + 1024 >= tar->cap) {
		void *newBase = malloc(tar->size + 1024);
		tar->cap += 1024;
		memcpy(newBase, tar->base, tar->size);
		free(tar->base);
		tar->base = newBase;
	}
	memset(tar->base + (tar->size - 1024), 0, 1024);
}


tar_entry_t *findTarFile(tar_t *tar, char *name) {
	tar_entry_t *centry;
	for (size_t i=0;i<tar->n_entries;i++) {
		centry = tar->entries[i];
		if (strcmp(centry->base, name)==0) {
			return centry;
		}
	}

	return NULL;
}
