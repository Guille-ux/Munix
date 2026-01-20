#ifndef MINIASM_LINK_H
#define MINIASM_LINK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "stream.h"

#define SYM_TABLE_GROW_FACTOR 2

typedef enum {
	SYM_END=0,
	SYM_LABEL=1,
	SYM_RELOCATION_REQUEST=2,
} SymbolType;

typedef struct {
	size_t id;
	size_t direction;
	bool ready;
	char *name;
} RelocationSymbol;

#define MAX_SYMBOL_NAME 33 // esto incluye '\0'

typedef struct {
	SymbolType type;
	size_t size; // tamaño 
	size_t id;
	size_t value; // dirección
	bool present;
	char name[MAX_SYMBOL_NAME];
} __attribute__((packed)) Symbol;

typedef struct {
	Symbol sym_table[];
} __attribute__((packed)) SymTable;

typedef struct {
	size_t count;
	size_t cap;
	Symbol sym_table[];
} __attribute__((packed)) LinkerTable;

void mapRel2Sym(RelocationSymbol *rel, LinkerTable *table);
void newRelocationRequest(ByteStream *stream, size_t pos, size_t size, const char *tag, size_t len);
void appendRelocationSymbol(ByteStream *stream, RelocationSymbol *rel);

#endif
