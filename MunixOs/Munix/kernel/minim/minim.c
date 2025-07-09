#include "../include/ksysarena.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include "../mbash/mbtype.h" // For isdigit, isspace

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

static char *m_strdup(const char *str) {
	size_t len = strlen(str);
	char *ret=(char*)kmalloc(len+1);
	strcpy(ret, str);
	ret[len]='\0';
	return ret;
}

// Define token types
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_PRINT,
    TOKEN_EOF,
    TOKEN_UNKNOWN // For error handling
} TokenType;

// Structure to hold a token
typedef struct {
    TokenType type;
    int value; // Only used for NUMBER tokens
    char *lexeme; // Original text of the token (useful for debugging/errors)
} Token;

// Basic lexer function
Token get_next_token(const char **source_code_ptr) {
    const char *current_char_ptr = *source_code_ptr;

    // Skip whitespace
    while (*current_char_ptr != '\0' && k_isspace((unsigned char)*current_char_ptr)) {
        current_char_ptr++;
    }

    *source_code_ptr = current_char_ptr; // Update source_code_ptr for next call

    if (*current_char_ptr == '\0') {
        return (Token){TOKEN_EOF, 0, NULL};
    }

    if (k_isdigit((unsigned char)*current_char_ptr)) {
        int value = 0;
        char *start = (char*)current_char_ptr;
        while (k_isdigit((unsigned char)*current_char_ptr)) {
            value = value * 10 + (*current_char_ptr - '0');
            current_char_ptr++;
        }
        size_t len = current_char_ptr - start;
        char *lexeme = (char*)kmalloc(len + 1);
        strncpy(lexeme, start, len);
        lexeme[len] = '\0';
        *source_code_ptr = current_char_ptr;
        return (Token){TOKEN_NUMBER, value, lexeme};
    }

    switch (*current_char_ptr) {
        case '+':
            *source_code_ptr = current_char_ptr + 1;
            return (Token){TOKEN_PLUS, 0, m_strdup("+")};
        case '-':
            *source_code_ptr = current_char_ptr + 1;
            return (Token){TOKEN_MINUS, 0, m_strdup("-")};
        case '*':
            *source_code_ptr = current_char_ptr + 1;
            return (Token){TOKEN_MULTIPLY, 0, m_strdup("*")};
        case '/':
            *source_code_ptr = current_char_ptr + 1;
            return (Token){TOKEN_DIVIDE, 0, m_strdup("/")};
        default:
            // Check for "PRINT" keyword
            if (strncmp(current_char_ptr, "PRINT", 5) == 0) {
                *source_code_ptr = current_char_ptr + 5;
                return (Token){TOKEN_PRINT, 0, m_strdup("PRINT")};
            }
            // If not a recognized character or keyword
            *source_code_ptr = current_char_ptr + 1;
            char *unknown_lexeme = (char*)kmalloc(2);
            unknown_lexeme[0] = *current_char_ptr;
            unknown_lexeme[1] = '\0';
            return (Token){TOKEN_UNKNOWN, 0, unknown_lexeme};
    }
}


// --- 2. Virtual Machine (Interpreter) ---

#define STACK_SIZE 256
int stack[STACK_SIZE];
int stack_ptr = 0; // Points to the next available slot

void push(int value) {
    if (stack_ptr < STACK_SIZE) {
        stack[stack_ptr++] = value;
    } else {
        kprintf("Stack overflow!\n");
    }
}

int pop() {
    if (stack_ptr > 0) {
        return stack[--stack_ptr];
    } else {
        kprintf("Stack underflow!\n");
    }
}

// Interpreter function
void interpret(const char *source_code) {
    const char *current_source_ptr = source_code;
    Token token;
    int a, b;

    do {
        token = get_next_token(&current_source_ptr);

        if (token.lexeme != NULL) {
            //printf("DEBUG: Token Type: %d, Lexeme: %s, Value: %d\n", token.type, token.lexeme, token.value);
        } else {
            //printf("DEBUG: Token Type: %d\n", token.type);
        }


        switch (token.type) {
            case TOKEN_NUMBER:
                push(token.value);
                break;
            case TOKEN_PLUS:
                b = pop();
                a = pop();
                push(a + b);
                break;
            case TOKEN_MINUS:
                b = pop();
                a = pop();
                push(a - b);
                break;
            case TOKEN_MULTIPLY:
                b = pop();
                a = pop();
                push(a * b);
                break;
            case TOKEN_DIVIDE:
                b = pop();
                a = pop();
                if (b == 0) {
                    kprintf("Division by zero!\n");
                }
                push(a / b);
                break;
            case TOKEN_PRINT:
                kprintf("%d\n", pop());
                break;
            case TOKEN_EOF:
                break; // End of input
            case TOKEN_UNKNOWN:
                kprintf("Unknown token: '%s'\n", token.lexeme);
        }
        if (token.lexeme != NULL) {
            kfree(token.lexeme); // Free memory allocated for lexeme
        }

    } while (token.type != TOKEN_EOF);
}

// --- Main function ---
int minim(const char *line) {
	interpret(line);

	return 0;
}
