/*
 * parser.c
 * sljaknsnak
 * es mi parser para mbash
 * 
 *
 *
 * No Se - Guillermo Leira Temes 2025
 *
 */

#include "parser.h"
#include "lexer.h"
#include "mbtype.h"
#include "../include/memory.h"

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

static Token *tokens;
static size_t token_index;

static ASTNode *parse_program_node();
static ASTNode *parse_statement();
static ASTNode *parse_asignment();
static ASTNode *parse_command_call();
static ASTNode *parse_export();
static ASTNode *parse_if();
static ASTNode *parse_while();
static ASTNode *parse_for();
static ASTNode *parse_break();
static ASTNode *parse_echo();
static ASTNode *parse_return();
static ASTNode *parse_expression();
static ASTNode *parse_unary_expr();
static ASTNode *parse_primary_expr();

static Token *peek() {
	return &tokens[token_index];
}

static Token *eat() {
	if (tokens[token_index].type==TOKEN_EOF) return &tokens[token_index];
	return &tokens[token_index++];
}

static bool match(TokenType type) {
	if (peek()->type != type) return false;
	token_index++;
	return true;
}

static ASTNode *newASTNode(ASTNodeType type) {
	ASTNode *node = (ASTNode *)kmalloc(sizeof(ASTNode));
	if (node == NULL) {
		kprintf("Err -> Out Of Memory!\n");
		return NULL;
	}
	node->type=type;
	memset(&node->data, 0, sizeof(node->data));
	return node;
}

static ASTNode *parse_program_node() {
	ASTNode *programNode=newASTNode(NODE_PROGRAM);
	if (programNode->type!=NODE_PROGRAM) return NULL;

	ProgramNode *program_data=&programNode->data.program;

	program_data->stmts=NULL;
	program_data->size=0;

	size_t cap=10;

	program_data->stmts=(ASTNode **)kmalloc(cap*sizeof(ASTNode*));

	if (!program_data->stmts) {
		kfree(programNode);
		return NULL;
	}

	while (peek()->type != TOKEN_EOF) {
		if (peek()->type == TOKEN_NEW_LINE) {
			eat();
			continue;
		}

		ASTNode *stmt = parse_statement();
		if (stmt==NULL) {
			kprintf("Err -> Memory Error!\n");
			break;
		}

		if (program_data->size >= cap) {
			cap *= 2;
			ASTNode **new_stmts=(ASTNode**)kmalloc(cap*sizeof(ASTNode*));
			if (!new_stmts) {
				kfree(programNode->data.program.stmts);
				kfree(programNode);
			}

			memcpy(new_stmts, program_data->stmts, program_data->size * sizeof(ASTNode*));

			kfree(program_data->stmts);
			program_data->stmts = new_stmts;
		}

		program_data->stmts[program_data->size++] == stmt;

		if (peek()->type != TOKEN_EOF && (peek()->type == TOKEN_NEW_LINE || peek()->type)) {
			eat();
		}

	}
	return programNode;
}

static ASTNode *parse_statement() {
	Token *current = peek();

	if (current->type==TOKEN_IDENTIFIER) {
		Token *next = &tokens[token_index+1];
		if (next->type==TOKEN_ASSIGN) {
			return parse_assignment();
		} else {
			return parse_command_call();
		}
	} else if (current->type == TOKEN_IF) {
		return parse_if();
	} else if (current->type == TOKEN_WHILE) {
		return parse_while();
	} else if (current->type == TOKEN_FOR) {
		return parse_for();
	} else if (current->type == TOKEN_BREAK) {
		return parse_break();
	} else if (current->type == TOKEN_RETURN) {
		return parse_return();
	} else if (current->type == TOKEN_EXPORT) {
		return parse_export();
	} else if (current->type == TOKEN_ECHO) {
		return parse_echo();
	}

	/*
 	 * Supongo que añadire más tipos de nodos, los pipes por ejemplo
 	 * estaria bien añadirlos
 	 *
 	 *
 	 */



	else {
		kprintf("Err -> Unknown Statement!\n");
		return NULL;
	}
}

void parser_init(Token *token_list) {
	tokens = token_list;
	token_index=0;
}

ASTNode *parse() {
	return parse_program();
}

void parser_free_ast(ASTNode *node) {
	if (node==NULL) return;

	switch (node->type) {
		case NODE_PROGRAM: {
					for (size_t i=0;i<node->data.program.size) {
						parser_free_ast(node->data.program.stmts[i]);
					}
					kfree(node->data.program.stmts);
					break;
				   }
		case NODE_ASSIGNMENT: {
					kfree(node->data.assignment.var_name);
					parser_free_ast(node->data.assignment.expr);
					break;
				      }
		case NODE_COMMAND_CALL: {
					kfree(node->data.command_call.name);
					for (size_t i=0;i<node->data.command_call.argc;i++) {
						parser_free_ast(node->data.command_call.args[i]);
					}
					kfree(node->data.command_call.args);
					break;
					}
		case NODE_VAR_REF: {
					kfree(node->data.var_ref.name);
					break;
				   }
		case NODE_STRING_LIT: {
					kfree(node->data.string_lit);
					break;
				      }
		case NODE_BINARY_EXPR: {
					parser_free_ast(node->data.binary_expr.left);
					parser_free_ast(node->data.binary_expr.right);
					break;
				       }
		case NODE_UNARY: {
					parser_free_ast(node->data.unary_expr.expr);
					break;
				 }
		case NODE_IF: {
	            			parser_free_ast(node->data.if_stmt.condition);
					for (size_t i = 0; i < node->data.if_stmt.s_then; i++) {
						parser_free_ast(node->data.if_stmt.thenb[i]);
					}
					kfree(node->data.if_stmt.thenb);
					for (size_t i = 0; i < node->data.if_stmt.s_else; i++) {
						parser_free_ast(node->data.if_stmt.elseb[i]);
					}
					kfree(node->data.if_stmt.elseb);
					break;
				}
		case NODE_WHILE_LOOP: {
					parser_free_ast(node->data.while_loop.condition);
					for (size_t i=0;i<node->data.while_loop.s_body; i++) {
						parser_free_ast(node->data.while_loop.body[i]);
					}
					kfree(node->data.while_loop.body);
					break;
				      }
		case NODE_FOR_LOOP: {
					parser_free_ast(node->data.for_loop.init);
					parser_free_ast(node->data.for_loop.condition);
					parser_free_ast(node->data.for_loop.inc);

					for (size_t i=0;i<node->data.for_loop.s_body;i++) {
						parser_free_ast(node->data.for_loop.body[i]);
					}
					kfree(node->data.for_loop.body);
					break;
				    }
		case NODE_ECHO: {
					parser_free_ast(node->data.echo_stmt.expr);
					break;
				}
		case NODE_EXPORT: {
					parser_free_ast(node->data.export_stmt.var);
					break;
				  }
		case NODE_RETURN: {
					parser_free_ast(node->data.ret_stmt.expr);
					break;
				  }
		default: break;
	}
	kfree(node);
}

ASTNode *parse_first_expr() {
	/*
	 * Empezando!!!
	 *
	 */
}
