#include "lexer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static const char *source;
static size_t current_pos;

static char advance() {
	if (source[current_pos]=='\0') return '\0';
	return source[current_pos++];
}

static char peek() {
	return source[current_pos];
}

static Token newToken(TokenType type, const char *start, size_t len) {
	Token token;
	token.type = type;
		
	char *value = (char *)kmalloc(len + 1);

	if (token.value == NULL) return token; // akjakjsklas, ERROR
	
	memcpy(token.value, start, len);
	
	value[len]='\0';
	token.value = value;
	return token;
}

void lexer_init(const char *source_code) {
	source=source_code;
	current_pos=0;
}

void lexer_free_token(Token *token) {
    if (token && token->value) {
        kfree(token->value);
        token->value = NULL;
    }
}
