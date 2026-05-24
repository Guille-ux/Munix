#ifndef MUNIX_KERNEL_USR_USR_H
#define MUNIX_KERNEL_USR_USR_H

#include <stdint.h>
#include <stdbool.h>

// aqui tengo el KID o Kernel Identity Document, para nada el nombre basado en DNI

typedef struct kid {
	int gid;
	int uid;
} kid_t;

typedef uint32_t permission_lvl_t;

#define IDENTITY_NAME_SIZE 128 - (sizeof(permission_lvl_t) + sizeof(bool)*2) 

typedef struct {
	char name[IDENTITY_NAME_SIZE];
	permission_lvl_t level;
	bool used;
	bool is_group; // solo para algunas cosas realmente	
} __attribute__((packed)) identity_t;

void initIdentities();
int createIdentity(permission_lvl_t lvl, bool is_group, char *name);
					   // es xq hubo un error
int removeIdentity(int iid);
identity_t *getIdentity(int iid); // función PELIGROSA,
				  // NO USAR A MENOS Q SEPAS LO Q HACES

#endif
