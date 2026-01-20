#ifndef MUNIXCC_PARSER_H
#define MUNIXCC_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "tokens.h"
#include "lexer.h"
#include "structs.h"
#include "utils.h"
#include "symbols.h"
#include "buffi.h"

extern char *name_base; // máximo 15 chars, (sin contar con '\0')
extern size_t tok_index;

int mcc_parse_program(TokenC *tokens, BufferI *buffer);

#endif
