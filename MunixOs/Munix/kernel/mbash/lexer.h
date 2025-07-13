#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "mbtype.h"
#include <stdbool.h>

extern bool debug;

/*
 * Todo esto es de un lenguaje de scripting que es solamente para hacer
 * funcionar la shell
 *
 *
 */

typedef enum {
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MINUS,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_FOR,
	TOKEN_BREAK,
	TOKEN_DOLLAR, // $ es para las variables, algo tipo $PATH
	TOKEN_WHILE,
	TOKEN_IF,
	TOKEN_FI,
	TOKEN_DONE,
	TOKEN_ECHO,
	TOKEN_LESS,
	TOKEN_GREATER,
	TOKEN_EXPORT,
	TOKEN_LEQ,
	TOKEN_GEQ,
	TOKEN_RETURN,
	TOKEN_BANG,
	TOKEN_DO,
	TOKEN_NEW_LINE,
	TOKEN_COMMA,
	TOKEN_DOT,
	TOKEN_COLON,
	TOKEN_NULL,
	TOKEN_BEQ,
	TOKEN_ELSE,
	TOKEN_THEN,
	TOKEN_PLUS,
	TOKEN_EQ,
	TOKEN_DPIPE,
	TOKEN_PIPE,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_NUMBER,
	TOKEN_ASSIGN,
	TOKEN_WHITESPACE,
	TOKEN_COMMENT,
	TOKEN_SEMICOLON,
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
Token *lexe(Token *buffer);
Token *lexen(Token *buffer, size_t n); // (para evitar buffer overflow)
void free_tokens(Token *buffer, size_t n);

#endif
