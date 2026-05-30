#ifndef FSD
#define FSD

#include <stdint.h>
#include <stddef.h>
#include "../disk/general.h"
#include "../partitions/partitions.h"
#include "../partitions/partition_mng.h"
#include "../disk/diski.h"
#include "fs.h"
#include "../usr/usr.h"

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

#define MAX_FD_NAME_SIZE 256

typedef struct file {
	char name[256];
	union {
		mfs_file_t mfs;
	} as;
	explorer_t *explorer;
	int (*read)(struct file *file, void *buffer, size_t size);
	int (*write)(struct file *file, void *buffer, size_t size);
	int (*extend)(struct file *file);
} file_t;

#define 

typedef struct fd {
	uint8_t flags; // para los permisos, no con los q se abre, pero para
		       // los del archivo, y ser más fácil gestionarlos
	

	uint32_t ref_count; // contador de referencias
	file_t file; // recordemos q file puede usarse para cualquier cosa
		     // realmente
} __attribute__((packed)) fd_t;


void initFd(); // inicializa la memoria donde se guardan los fd's 

int createFd(permission_lvl_t lvl, bool is_group, char *name);
					   // es xq hubo un error
int removeFd(int fid);
identity_t *getFd(int fid); // función PELIGROSA,
				  // NO USAR A MENOS Q SEPAS LO Q HACES
#endif
