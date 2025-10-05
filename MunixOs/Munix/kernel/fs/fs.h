#ifndef FILE_SYSTEM
#define FILE_SYSTEM

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../partitions/partitions.h"
#include "mfs.h"

struct explorer_t;

typedef struct {
	mfs_superblock_t *superblock;
	void *ifat_table;
} mfs_meta_t;

typedef struct {
	char name[256];
} file_item_t;

struct files_t;

typedef struct files_t {
	uint32_t current_idx;
	explorer_t *fs;

	int (*next)(struct files_t *iterator, file_item_t *out_item);
	uint32_t (*count)(struct files_t *iterator);
} files_t;

typedef struct explorer_t {
	uint16_t owner_id;
	uint16_t group_id;

	void *meta;

	char path[256];

	void **cwd; // Ahora es un handle para mayor flexibilidad

	partition_t *partition;

	int (*ls)(struct explorer_t *exp, files_t *list);
	int (*cd)(struct explorer_t *exp, const char *dir_name);
	int (*mkdir)(struct explorer_t *exp, const char *name);
	int (*mod)(struct explorer_t *exp, uint16_t *permissions, uint16_t *group, uint16_t *owner);
	int (*chmod)(struct explorer_t *exp, uint16_t permissions, uint16_t group, uint16_t owner);
	int (*stat)(struct explorer_t *exp, const char *name, uint8_t *attr);
	int (*chstat)(struct explorer_t *exp, const char *name, uint8_t attr);
	int (*size)(struct explorer_t *exp, const char *name, uint32_t *size);
	int (*time)(struct explorer_t *exp, const char *name, uint32_t *mod);
	int (*chtime)(struct explorer_t *exp, const char *name, uint32_t mod);

	int (*touch)(struct explorer_t *exp, const char *name, uint32_t size);
	int (*cat)(struct explorer_t *exp, const char *name, void *buffer, uint32_t amount);
	int (*write)(struct explorer_t *exp, const char *name, void *content, uint32_t n);
	int (*remove)(struct explorer_t *exp, const char *name);
	

	int (*clean)(struct explorer_t *exp); // específica para mfs, limpiar tomsbtones, se podra usar para limpiar el sistema de archivos
	int (*destroy)(struct explorer_t *exp); // liberar memoria	
	/* 
	 * Futuro: crear manejadores de archivos
	 */
} explorer_t;

explorer_t *mfs_init_explorer(partition_t *partition, explorer_t *explorer, uint16_t owner_id, uint16_t group_id);

/*
 * Funciones para MunixFS
 *
 */

int mfs_ls(explorer_t *explorer, files_t *list);

/*
 * Subfunciones de LS
 */

int mfs_ls_next(files_t *iterator, file_item_t *item);
uint32_t mfs_ls_count(files_t *iterator);

/*
 * Fin de las subfunciones de LS
 */

int mfs_cd(explorer_t *explorer, const char *dir_name);
int mfs_mkdir(explorer_t *explorer, const char *name);
int mfs_mod(explorer_t *explorer, uint16_t *permissions, uint16_t *group, uint16_t *owner);
int mfs_chmod(explorer_t *explorer, uint16_t permissions, uint16_t group, uint16_t owner);
int mfs_stat(explorer_t *explorer, const char *name, uint8_t *attr);
int mfs_chstat(explorer_t *explorer, const char *name, uint8_t attr);
int mfs_size(explorer_t *explorer, const char *name, uint32_t *size);
int mfs_time(explorer_t *explorer, const char *name, uint32_t *mod);
int mfs_chtime(explorer_t *explorer, const char *name, uint32_t mod);
int mfs_touch(explorer_t *explorer, const char *name, uint32_t size);
int mfs_cat(explorer_t *explorer, const char *name, void *buffer, uint32_t amount);
int mfs_write(explorer_t *explorer, const char *name, void *content, uint32_t n);
int mfs_remove(explorer_t *explorer, const char *name);

int mfs_clean(explorer_t *explorer);

int mfs_destroy(explorer_t *explorer);

#endif
