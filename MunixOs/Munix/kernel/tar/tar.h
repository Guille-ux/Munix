#ifndef MUNIX_TAR_H
#define MUNIX_TAR_H

#include <stdint.h>

/*
 *
 * Definiciones para tar, tar lo usare para cosas como archivos de linker etc
 * sera muy útil
 *
 */

// la estructura salio de osdev
typedef struct {
	char name[100];
	uint8_t mode[8];
	uint8_t uid[8];
	uint8_t gid[8];
	uint8_t size[12];
	uint8_t mtime[12];
	uint8_t chksum[8];
	uint8_t typeflag;
} tar_header_t;

#define TAR_BLOCK_SIZE 512

size_t oct2int(char *chain, char length);
void int2oct(char *chain, char length, size_t n);

// ahora toca definir funciones para más cosas y otras estructuras
// bueno, allá voy!

typedef struct {
	void *base; // como el nombre es lo primero sirve tambien para pillar
		    // el nombre
	size_t size;
	size_t gid;
	size_t uid;
} tar_entry_t;

typedef struct {
	void *start;
	size_t n_entries;
	tar_entry_t *entries;
} tar_t;

tar_t *scanTarFile(tar_t *tar, void *block);

// más tarde añadire las funciones para crear tar's

#endif
