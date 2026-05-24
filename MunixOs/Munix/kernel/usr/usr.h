#ifndef MUNIX_KERNEL_USR_USR_H
#define MUNIX_KERNEL_USR_USR_H

#include <stdint.h>
#include <stdbool.h>

// aqui tengo el KID o Kernel Identity Document, para nada el nombre basado en DNI

typedef struct kid {
	uint32_t gid;
	uint32_t uid;
} kid_t;

typedef uint32_t permission_lvl_t;

typedef struct identities {
	permission_lvl_t level;
	bool free;
} identity_t

void initIdentities();

#endif
