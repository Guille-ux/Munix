#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "mbtype.h"

/*
 * Todo esto es de un lenguaje de scripting que es solamente para hacer
 * funcionar la shell
 *
 *
 */

typedef enum {
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_NUMBER,
	TOKEN_ASSIGN,
	TOKEN_WHITESPACE,
	TOKEN_COMMENT,
	/*
	 * Añadir más Tokens supongo
	 * xD
	 */ 
	TOKEN_EOF,
	TOKEN_UNKNOWN,
} TokenType;

typedef struct {
	TokenType type;
	char *value;
} Token;

void lexer_init(const char *source_code);
Token lexer_next_token();
void lexer_free_token(Token *token);

#endif
