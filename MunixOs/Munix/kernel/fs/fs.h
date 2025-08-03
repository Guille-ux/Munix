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

typedef struct explorer_t {
	uint16_t owner_id;
	uint16_t group_id;

	void *meta;

	char path[256];

	void *cwd;

	partition_t *partition;
	
	int (*cd)(struct explorer_t *exp, const char *dir_name);
	int (*mkdir)(struct explorer_t *exp, const char *name);
	int (*mod)(struct exporer_t *exp, uint16_t *permissions, uint16_t *group, uint16_t *owner);
	int (*chmod)(struct explorer_t *exp, uint16_t permissions, uint16_t group, uint16_t owner);
	int (*stat)(struct explorer_t *exp, const char *name, uint8_t *attr);
	int (*chstat)(struct explorer_t *exp, const char *name, uint8_t attr);

	int (*touch)(struct explorer_t *exp, const char *name);
	int (*cat)(struct explorer_t *exp, const char *name);
	int (*write)(struct explorer_t *exp, const char *name, void *content, uint32_t n);
	int (*remove)(struct exporer_t *exp, const char *name);
} exporer_t;

#endif
