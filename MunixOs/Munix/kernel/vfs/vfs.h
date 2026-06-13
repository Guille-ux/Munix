#ifndef MUNIX_VFS_H
#define MUNIX_VFS_H

/*
 * No creas q es nada super complejo, solo una pequeña estructura y algo de
 * código q mejoran las funciones para moverse y le dice al kernel
 * una forma de acceder al disco principal sin muchos problemas
 * nada más, en el futuro ya hare una cache de inodos y cosas raras
 *
 */

#include "../fs/fs.h"
#include "../tar/tar.h"
#include <stdint.h>
#include <stddef.h>

typedef enum {
	TAR_FS,
	EXPLORER_FS,
} vfs_type;

typedef struct {
	vfs_type type;

	union {
		explorer_t *expfs;
		tar_t *tarfs;
	} fs
} vfs_t;

extern vfs_t kernel_vfs;

int initKernelVFS_EXP(vfs_type type, explorer_t *expfs);
int ucd(const char *names); // better cd
int ufd(const char *names); // better fd creator

#endif
