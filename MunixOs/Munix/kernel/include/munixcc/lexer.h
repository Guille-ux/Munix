#ifndef MUNIXCC_LEXER_H
#define MUNIXCC_LEXER_H

/*
 *
 * Código que ira leyendolo todo y lo convertira a un array de tokens
 *
 */

#include "compat.h"
#include "tokens.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


// funciones auxiliares
bool munixccPattern(const char *pattern);
TokenC newTokC(char *start, size_t len, int line, TokenTypeC type);
bool munixccLex(const char *text, TokenC *tokens, size_t max_tokens);

#endif
