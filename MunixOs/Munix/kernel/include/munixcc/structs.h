#ifndef MUNIXCC_STRUCTS_H
#define MUNIXCC_STRUCTS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define MCC_STRUCTS_TABLE_SIZE 256 // cambialo para permitir los que quieras
#define MCC_STRUCT_MAX_NAME 32 // cambialo a lo que quieras
#define MCC_STRUCT_MAX_MEMBERS 32

typedef struct {
	char name[MCC_STRUCT_MAX_NAME];
	size_t offset;
	size_t size;
} MCCStructMember;

typedef struct {
	char name[MCC_STRUCT_MAX_NAME];
	MCCStructMember members[MCC_STRUCT_MAX_MEMBERS];
	int num_members;
	size_t size;
} MCCStructDef;

typedef struct {
	MCCStructDef structs[MCC_STRUCTS_TABLE_SIZE];
	size_t current_struct;
} MCCStructTable;

extern MCCStructTable CStructTable;

void mcc_CreateNewStruct(const char *name, ...); // el último argumento
						 // debe ser NULL
						 // los argumentos
						 // son punteros
						 // a miembros
						 // del resto se encarga
						 // la función

MCCStructDef *mcc_FindStruct(const char *name);
MCCStructMember *mcc_FindMember(const char *name, MCCStructDef *truct);

#endif
