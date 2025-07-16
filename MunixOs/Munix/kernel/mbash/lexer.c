#include "lexer.h"
#include "../include/memory.h"
#include "../include/ksysarena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

bool debug=false;
static const char *source;
static size_t current_pos;

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

static char *m_strndup(const char *str, size_t n) {
	char *ret=(char*)kmalloc(n+1);
	memcpy((void*)ret, (const void*)str, n);
	ret[n]='\0';
	return ret;
}

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

	

	token.value=m_strndup(start, len);
	if (debug==true) {
		kprintf("New Token -> %s \n", (const char *)token.value);
	}
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

/*
 * Es un lexer muy simple, pretendo mejorarlo, esto es provisional
 * lo justo y necesario para la shell.
 *
 * Estoy deseando sustituir minim por esto!!!!
 *
 * Guillermo Leira Temes - 2025
 */

static bool match(char c) {
	if (peek()==c) {
		advance();
		return true;
	} 
	return false;
}

static bool pattern(const char *str) {
	size_t last = current_pos;
	const char *s=str;
	bool tmp=false;
	while (match(*s++)) {
		if (*s=='\0') tmp=true;
	}
	if (!tmp) {
		current_pos = last;
		return false;
	}
	advance();
	/*
	 * Creo que esto es suficiente para comprobarlo todo, ¿no?
	 *
	 */
	
	if (peek()<'Z' && peek()>'A') return false;
	if (peek()<'z' && peek()>'a') return false;
	if (peek()=='_') return false;

	return true;
}

Token lexer_next_token() {
	char c;
	while (peek() != '\0') {
		c = peek();
		switch (c) {
			case '\n': advance(); return newToken(TOKEN_NEW_LINE, "\n", 1);
			case '*': advance(); return newToken(TOKEN_MUL, "*", 1);
			case '/': advance(); return newToken(TOKEN_DIV, "/", 1);
			case '-': advance(); return newToken(TOKEN_MINUS, "-", 1);
			case '(': advance(); return newToken(TOKEN_LPAREN, "(", 1);
			case ')': advance(); return newToken(TOKEN_RPAREN, ")", 1);
			case '[': advance(); return newToken(TOKEN_LBRACKET, "[", 1);
			case ']': advance(); return newToken(TOKEN_RBRACKET, "]", 1);
			case '{': advance(); return newToken(TOKEN_LBRACE, "{", 1);
			case '}': advance(); return newToken(TOKEN_RBRACE, "}", 1);
			case '$': advance(); return newToken(TOKEN_DOLLAR, "$", 1);
			case '+': advance(); return newToken(TOKEN_PLUS, "+", 1);
			case ';': advance(); return newToken(TOKEN_SEMICOLON, ";", 1);
			case ',': advance(); return newToken(TOKEN_COMMA, ",", 1);
			case ':': advance(); return newToken(TOKEN_COLON, ":", 1);
			case '.': advance(); return newToken(TOKEN_DOT, ".", 1);
			case '!': advance(); return newToken(TOKEN_BANG, "!", 1);
			case '<': advance(); return newToken(TOKEN_LESS, "<", 1);
			case '>': advance(); return newToken(TOKEN_GREATER, ">", 1);
			case '|': {
				advance();
				if (peek()=='|') {
					advance();
					return newToken(TOKEN_DPIPE, "||", 2);
				} else {
					return newToken(TOKEN_PIPE, "|", 1);
				}
				  }
			default: break;
		}
		if (k_isspace(c)) {
			while (k_isspace(peek())) advance();
			continue;
		}
		if (c=='#') {
			while (peek() != '\n' && peek() != '\0') advance();
			continue;
		}
		if (c=='\'') {
			advance();
			size_t start_pos=current_pos;
			while (peek() != '\'' && peek() != '\0') {
				advance();
			}
			if (peek() == '\0') return newToken(TOKEN_UNKNOWN, "Unterminated String!", 20);
			Token string_tok=newToken(TOKEN_STRING, source + start_pos, current_pos - start_pos);
			advance();
			return string_tok;
		}
		if (c=='"') {
			advance();
			size_t start_pos=current_pos;
			while (peek() != '"' && peek() != '\0') {
				advance();
			}
			if (peek() == '\0') return newToken(TOKEN_UNKNOWN, "Unterminated String!", 20);

			Token string_tok=newToken(TOKEN_STRING, source + start_pos, current_pos-start_pos);
			advance();
			return string_tok;
		}
		if (c=='=') {
			advance();
			if (peek()=='=') {
				advance();
				return newToken(TOKEN_EQ, "==", 2);
			}
			return newToken(TOKEN_ASSIGN, "=", 1);
		}
		
		if (k_isdigit(c)) { // NOTA: añadir soporte para decimales, en el futuro
			size_t start_pos=current_pos;
			while (k_isdigit(peek())) advance();
			return newToken(TOKEN_NUMBER, source + start_pos, current_pos - start_pos);
		}

		/*
		 * Patterns, son tokens internos, por ejemplo para break
		 * xD
		 */
		if (pattern("for")) return newToken(TOKEN_FOR, "for", 3);
		if (pattern("break")) return newToken(TOKEN_BREAK, "break", 5);
		if (pattern("if")) return newToken(TOKEN_IF, "if", 2);

		if (pattern("fi")) return newToken(TOKEN_FI, "fi", 2);
		if (pattern("while")) return newToken(TOKEN_WHILE, "while", 5);
		if (pattern("else")) return newToken(TOKEN_ELSE, "else", 4);
		if (pattern("then")) return newToken(TOKEN_THEN, "then", 4);
		if (pattern("echo")) return newToken(TOKEN_ECHO, "echo", 4);
		if (pattern("export")) return newToken(TOKEN_EXPORT, "export", 6);
		if (pattern("<=")) return newToken(TOKEN_LEQ, "<=", 2);
		if (pattern(">=")) return newToken(TOKEN_GEQ, ">=", 2);
		if (pattern("return")) return newToken(TOKEN_RETURN, "return", 6);
		if (pattern("null")) return newToken(TOKEN_NULL, "null", 4);
		if (pattern("!=")) return newToken(TOKEN_BEQ, "!=", 2);
		if (pattern("do")) return newToken(TOKEN_DO, "do", 2);
		if (pattern("done")) return newToken(TOKEN_DONE, "done", 4);

		/*
		 * Añadir más tokens de este tipo, algo me dice que seran importantes
		 * Podría poner aqui los comandos internos, no se
		 * Creo que no, que los maneje la shell
		 *
		 */

		if (k_isalpha(c) || c=='_') {
			size_t start_pos = current_pos;
			while (k_isalnum(peek()) || peek() == '_') advance();
			return newToken(TOKEN_IDENTIFIER, source + start_pos, current_pos - start_pos);
		}
		c = advance();
		return newToken(TOKEN_UNKNOWN, &c, 1);
	}
	return newToken(TOKEN_EOF, "", 0);
}

Token *lexe(Token *buffer) {
	Token *b=buffer;

	while ((b-1)->type!=TOKEN_EOF) {
		*b=lexer_next_token();
		if (debug) {
			kprintf("Token: %s\n", b->value);
			kprintf("Token Type: %d\n", (int)b->type);
		}
		b++;
	}

	return buffer;
}

Token *lexen(Token *buffer, size_t n) {
	Token *b=buffer;
	while ((b-1)->type!=TOKEN_EOF && n-- > 0) {
		*b=lexer_next_token();
		if (debug) {
			kprintf("Token: %s\n", b->value);
			kprintf("Token Type: %d\n", (int)b->type);
		}
		b++;
	}
	return buffer;
}
void free_tokens(Token *buffer, size_t n) {
	while (n-- > 0) {
		kfree(buffer[n].value);
		buffer[n].value=NULL;
		buffer[n].type = TOKEN_UNKNOWN;
	}
}
