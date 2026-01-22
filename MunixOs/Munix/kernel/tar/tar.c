#include "tar.h"

size_t oct2int(char *chain, char length) {
	size_t size = 0;
	
	for (int i=0;i < length-1;i++) {
		size += chain[i] - '0';
		size *= 8;
	}

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
		p += entries[n_entries-1]+TAR_BLOCK_SIZE;
	}

	return tar;
}
