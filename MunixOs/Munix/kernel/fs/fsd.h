#ifndef FSD
#define FSD

#include <stdint.h>
#include <stddef.h>
#include "../disk/general.h"
#include "../partitions/partitions.h"
#include "../partitions/partition_mng.h"
#include "../disk/diski.h"
#include "fs.h"

// esto contiene el descriptor de archivos, q permitira un uso más sencillo
// de los archivos, vamos no habra q usar el explorador

typedef struct {
	uint32_t first_block;
	uint32_t size_in_blocks;
	uint32_t modified;
	uint8_t attr;
	uint32_t folder_size;
	uint32_t folder_block;
} mfs_file_t;

struct file;

typedef struct file {
	char name[256];
	union {
		mfs_file_t mfs;
	} as;
	explorer_t *explorer;
	int (*read)(file *file, void *buffer, size_t size);
	int (*write)(file *file, void *buffer, size_t size);
	int (*extend)(file *file);
} file_t;



#endif
