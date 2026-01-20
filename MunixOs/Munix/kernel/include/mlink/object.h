#ifndef MLINK_OBJECT_H
#define MLINK_OBJECT_H

#include <stddef.h>
#include <stdint.h>
#include "link.h"

typedef enum {
	BLOB_METADATA,
	BLOB_RAW_CODE,
} BlobType;

typedef struct {
	BlobType type;
	size_t size;
	size_t offset; // offset respecto al inicio del primer blob
	uint8_t *blob; // Un puntero al blob binario
} Blob;

typedef struct {
	size_t n_blobs;
	Blob blobs[];
} Object;

typedef struct {
	size_t val;
	bool found;
} ResolvRef;

int mlink_resolv(size_t blob_n, Object *meta, Object *Raw);
ResolvRef searchInBlob(Blob *blob, Symbol *sym);

#endif
