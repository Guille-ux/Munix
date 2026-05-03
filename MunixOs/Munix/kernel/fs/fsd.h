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
	size_t start_block;
} mfs_file_t;

struct file;

typedef struct file {
	char name[256];
	void *meta; // esto dira cosas como donde se encuentra etc
	void (*read)(file *file, void *buffer, size_t size);
	void (*write)(file *file, void *buffer, size_t size);
	void (*extend)(file *file);
} file_t;

#endif
