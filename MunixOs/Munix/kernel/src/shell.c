#include "../include/libcs2.h"
#include "../include/shell.h"
#include "../include/memory.h"
#include "../disk/diski.h"
#include "../mbash/lexer.h"
#include "../mbash/parser.h"
#include "../mbash/eval.h"
#include "../minim/minim.h"
#include <stdint.h>
#include <stdbool.h>

bool send=false;
bool backspace=false;
bool shell_event=false;
char *shell_prompt="~ MunixOs ~> ";
char shell_buffer[SHELL_BUFFER_SIZE];
uint32_t shell_index=0;

void push_to_buffer(char c) {
    if (!(shell_index<SHELL_BUFFER_SIZE)) return;
    shell_buffer[shell_index++]=c;
    shell_buffer[shell_index]='\0';
}

void pop_from_buffer() {
    if (shell_index==0) return;
    shell_buffer[--shell_index]='\0';
}

void shell_update() {
    kprintf("\r");
    for (int i=1;i<stdout_interface.get_max_x()/8;i++) {
        kprintf("\t");
    }
    kprintf("\r%s %s", (const char*)shell_prompt, (const char*)shell_buffer);
}

#define MAX_TOKENS 64

int shellEntry() {
	__asm__ volatile("cli");
	EvalCtx *global_ctx = newShellCtx();
	Token *t_buff = (Token*)kmalloc(MAX_TOKENS*sizeof(Token));
	__asm__ volatile("sti");
	shell_update();
	while (1) {
		if (shell_event) {
			push_to_buffer(final_character);
			shell_update();
			shell_event=false;
		} else if (backspace) {
			pop_from_buffer();
			shell_update();
			backspace=false;
		} else if (send) {
			kprintf("\n");
			
			// Análisis Léxico
			lexer_init(shell_buffer);
			lexen(t_buff, MAX_TOKENS);
			
			// Parseo
			parser_init(t_buff);
			ASTNode *tree = parse();

			// Evaluación
			eval(tree, global_ctx);

			// Liberar Memoria
			parser_free_ast(tree);
			//free_tokens(t_buff, MAX_TOKENS);
			
			// Reiniciar flags e index
			send=false;
			shell_index=0;

			// Reiniciar el buffer
			memset(shell_buffer, 0, sizeof(char)*SHELL_BUFFER_SIZE);
			shell_update(); // Actualizar Prompt
		}

		__asm__ volatile("hlt"); // estado de bajo consumo 
	}
	return 0;
}
