#ifndef MUNIXCC_SYMBOL_TABLE
#define MUNIXCC_SYMBOL_TABLE

#include "compat.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffi.h"
#include "tokens.h"
#include "utils.h"

#define MCC_MAX_SYMBOLS 256 // puedes cambiarlo por lo que quieras
#define MCC_MAX_SYMBOL_NAME 32 // puedes cambiarlo por lo que quieras
#define MCC_STACK_OFFSET 4 // para que sean 4 bytes



typedef struct {
	char name[MCC_MAX_SYMBOL_NAME];
	char _type_[MCC_MAX_SYMBOL_NAME]; // algo principalmente informativo
	
	// los esgtructs fuera
	// no se
	//bool is_struct; // si es una struct, seria conveniente saberlp
	size_t size;
	size_t offset; // el offset es siempre negativo, pero guardamos lo
		       // positivo
	size_t nscope; // nivel de scope
	bool is_ptr;
	bool global;
} MCC_Var;

typedef struct {
	MCC_Var table[MCC_MAX_SYMBOLS];
	size_t symbol_count;
	size_t current_scope;
	size_t current_stack_offset;
} MCC_VarTable;

typedef struct {
	MCC_Var table[MCC_MAX_SYMBOLS];
	size_t symbol_count;
} MCC_g_Table;

extern MCC_g_Table CGlobalTable;
extern MCC_VarTable CVarTable;

void mcc_add_g(MCC_Var var);
void mcc_push_var(MCC_Var var);
void mcc_clean_tab(void); // para quitar lo que salio de scope
MCC_Var *mcc_find_var(char *name);





// otras partes....

int handle_identifier(TokenC *tokens, BufferI *buffer);
size_t handle_address(TokenC *tokens, BufferI *buffer);

#endif
