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

static ASTNode *parse_program();
static ASTNode *parse_statement();
static ASTNode *parse_assignment();
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
static ASTNode **parse_statement_list(size_t *out_s, TokenType *stop_tokens, size_t n_stop_tokens);

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

static ASTNode *parse_program() {
	ASTNode *programNode=newASTNode(NODE_PROGRAM);
	if (programNode==NULL) return NULL;

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
			parser_free_ast(programNode);
			return NULL;
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

		program_data->stmts[program_data->size++] = stmt;

		if (peek()->type != TOKEN_EOF && (peek()->type == TOKEN_NEW_LINE)) {
			eat();
		}

	}
	return programNode;
}

static ASTNode *parse_statement() {
	Token *current = peek();

	if (current->type==TOKEN_IDENTIFIER) {
		return parse_command_call();
	else if (current->type==TOKEN_NUMBER) {
		return parse_assignment();
	} else if (current->type == TOKEN_IF) {
		return parse_if();
	} else if (current->type == TOKEN_DO) {
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
					for (size_t i=0;i<node->data.program.size;i++) {
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

static ASTNode *parse_primary_expr() {
	/*
	 * Empezando!!!
	 *
	 */
	ASTNode *new_node;
	Token *current = peek();
	switch (current->type) {
		case TOKEN_NUMBER: {
			eat();
			new_node = newASTNode(NODE_NUM_LIT);
			new_node->data.number_lit = atol((const char*)current->value);
			lexer_free_token(current);
			break;
				   }
		case TOKEN_STRING: {
			eat();
			new_node = newASTNode(NODE_STRING_LIT);
			new_node->data.string_lit = strdup((const char*)current->value);
			lexer_free_token(current);
			break;
				   }
		case TOKEN_DOLLAR: {
			lexer_free_token(current);
			eat();
			current = peek();
			if (!match(TOKEN_NUMBER)) return NULL;
			size_t number = atol(current->value);
			lexer_free_token(current);
			current = peek();
			if (current->type==TOKEN_COLON) {
				new_node=newASTNode(NODE_VAR_REF);
				lexer_free_token(current);
				eat();
				current = peek();
				if (current->type!=TOKEN_IDENTIFIER) return NULL;
				new_node->data.var_ref.name = strdup((const char*)current->value);
				new_node->data.var_ref.num=number;
			} else {
				new_node=newASTNode(NODE_ARG_REF);
				new_node->data.arg_ref.num=number;
			}
			lexer_free_token(current);
			break;
				   }
		case TOKEN_LPAREN: {
			lexer_free_token(current);
			current = eat();
			new_node = parse_expression();
			if (!match(TOKEN_RPAREN)) {
				kprintf("Err -> Expected ')'!\n");
				return NULL;
			}
			break;
				   }
		default: {
			kprintf("Err -> Unexpected Token!\n");
			return NULL;
			 }
	}
	eat();
	return new_node;
}

static ASTNode *parse_unary_expr() {
	Token *current = peek();
	ASTNode *new_node;
	switch (current->type) {
		case TOKEN_MINUS: {
			lexer_free_token(current);
			eat();
			current = peek();
			new_node = newASTNode(NODE_UNARY);
			if (new_node==NULL) return NULL;
			new_node->data.unary_expr.expr = parse_unary_expr();
			new_node->data.unary_expr.op = TOKEN_MINUS;
			break;
				  }
		case TOKEN_BANG: {
			lexer_free_token(current);
			eat();
			current = peek();
			new_node = newASTNode(NODE_UNARY);
			if (new_node==NULL) return NULL;
			new_node->data.unary_expr.expr = parse_unary_expr();
			new_node->data.unary_expr.op = TOKEN_BANG;
			break;
				 }
		default: {
			new_node = parse_primary_expr();
			break;
			 }
	}
	return new_node;
}

/*
 * Definiciones de funciones de la cadena de parseo para expresiones
 *
 */

static ASTNode *parse_factor();
static ASTNode *parse_term();


static ASTNode *parse_factor() {
	return parse_unary_expr();
}

static ASTNode *parse_term() {
	ASTNode *left_node = parse_factor();
	while ((peek()->type == TOKEN_MUL || peek()->type == TOKEN_DIV)) {
		Token *op = eat();

		ASTNode *right_node=parse_factor();
		if (right_node==NULL) {
			parser_free_ast(left_node);
			return NULL;
		}
		ASTNode *binExpr = newASTNode(NODE_BINARY_EXPR);
		if (binExpr == NULL) {
			parser_free_ast(left_node);
			parser_free_ast(right_node);
			return NULL;
		}

		binExpr->data.binary_expr.op=op->type;
		binExpr->data.binary_expr.left=left_node;
		binExpr->data.binary_expr.right=right_node;

		left_node = binExpr;
	}
	return left_node;
}

static ASTNode *parse_expression() {
	ASTNode *left_node = parse_term();
	while  ((peek()->type == TOKEN_PLUS ||
		peek()->type == TOKEN_MINUS ||
		peek()->type == TOKEN_EQ ||
		peek()->type == TOKEN_GT ||
		peek()->type == TOKEN_LT ||
		peek()->type == TOKEN_LEQ ||
		peek()->type == TOKEN_GEQ ||
		peek()->type == TOKEN_BEQ)) {
		Token *op = eat();

		ASTNode *right_node=parse_term();
		if (right_node==NULL) {
			parser_free_ast(left_node);
			return NULL;
		}
		ASTNode *binExpr = newASTNode(NODE_BINARY_EXPR);
		if (binExpr == NULL) {
			parser_free_ast(left_node);
			parser_free_ast(right_node);
			return NULL;
		}

		binExpr->data.binary_expr.op=op->type;
		binExpr->data.binary_expr.left=left_node;
		binExpr->data.binary_expr.right=right_node;

		left_node = binExpr;
	}
	return left_node;
}

static ASTNode *parse_assignment() {
	ASTNode *new_node=newASTNode(NODE_ASSIGNMENT);
	if (new_node==NULL) {
		return NULL;
	}
	Token *number = eat();
	if (!match(TOKEN_COLON)) {
		kprintf("Err -> Expected ':' after number and before identifier\n");
		parser_free_ast(new_node);
		lexer_free_token(number);
		return NULL;
	}
	Token *name = eat();
	if (number->type!=TOKEN_NUMBER) {
		parser_free_ast(new_node);
		lexer_free_token(number);
		lexer_free_token(name);
		return NULL;
	}
	if (name->type!=TOKEN_STRING) {
		parser_free_ast(new_node);
		lexer_free_token(name);
		lexer_free_number();
		return NULL;
	}
	new_node->data.assignment.num = atol(number->value);
	new_node->data.assignment.var_name = strdup(name->value);
	lexer_free_token(name);
	lexer_free_token(number);
	if (!match(TOKEN_ASSIGN)) {
		kprintf("Err -> Expected '=' after identifier\n");
		parser_free_ast(new_node);
		return NULL;
	}
	ASTNode *expr_node = parse_expression();
	if (expr_node==NULL) {
		parser_free_ast(expr_node);
		parser_free_ast(new_node);
		return NULL;
	}
	new_node->data.assignment.expr = expr_node;
	return new_node;
}

static ASTNode **parse_expression_list(size_t *argc);

static ASTNode *parse_command_call() {
	ASTNode *new_node = newASTNode(NODE_COMMAND_CALL);
	if (new_node==NULL) return NULL;

	Token *cmd = eat();
	if (cmd->type != TOKEN_IDENTIFIER) {
		kprintf("Err -> Expected Identifier but found Token n. %d\n", (int)cmd->type);
		parser_free_ast(new_node);
		lexer_free_token(cmd);
		return NULL;
	}
	new_node->data.command_call.name = strdup(cmd->value);
	if (new_node->data.command_call.name==NULL) {
		kprintf("Err -> Out of Memory Error!\n");
		parser_free_ast(new_node);
		lexer_free_token(cmd);
		return NULL;
	}
	lexer_free_token(cmd);


	size_t argc=0; // args count
	
	ASTNode **argv = parse_expression_list(&argc);

	if (argv==NULL) {
		kprintf("Err -> Out of Memory\n");
		parser_free_ast(new_node);
		return NULL;
	}
	new_node->data.command_call.argc=argc;
	new_node->data.command_call.args=argv;
	return new_node;
}

static ASTNode **parse_expression_list(size_t *argc) {
	ASTNode **list = NULL;
	size_t size=0;
	size_t cap = 4;

	list = (ASTNode**)kmalloc(sizeof(ASTNode*)*cap);
	if (list==NULL) {
		kprintf("Err -> Out of Memory\n");
		*argc=0;
		return NULL;
	}

	while  (peek()->type != TOKEN_NEW_LINE &&
		peek()->type != TOKEN_EOF &&
		peek()->type != TOKEN_PIPE &&
		peek()->type != TOKEN_IF &&
		peek()->type != TOKEN_WHILE &&
		peek()->type != TOKEN_FOR &&
		peek()->type != TOKEN_BREAK &&
		peek()->type != TOKEN_RETURN &&
		peek()->type != TOKEN_EXPORT &&
		peek()->type != TOKEN_ECHO) {
		

		ASTNode *arg = parse_expression();
		if (arg == NULL) {
			kprintf("Err -> Out of Memory\n");
			for (size_t i = 0;i<size;i++) {
				parser_free_ast(list[i]);
			}
			kfree(list);
			*argc = 0;
			return NULL;
		}
		if (size >= cap) {
			cap *= 2;
			ASTNode **new_list = (ASTNode **)kmalloc(sizeof(ASTNode*)*cap);
			if (new_list == NULL) {
				kprintf("Err -> Out of Memory\n");
				parser_free_ast(arg);
				for (size_t i = 0;i<size;i++) {
					parser_free_ast(list[i]);
				}
				*argc = 0;
				kfree(list);
				return NULL;
			}
		}
		memcpy((void*)new_list, (const void*)list, size * sizeof(ASTNode *));
		kfree(list);
		list = new_list;
		list[size++]=arg;
	}
	*argc = size;
	return list;
}

static ASTNode *parse_echo() {
	ASTNode *new_node = newASTNode(NODE_ECHO);

	if (new_node == NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}
	Token *echo_k = eat();
	lexer_free_token(echo_k);
	ASTNode *expr = parse_expression();
	if (expr == NULL) {
		kfree(new_node);
		return NULL;
	}
	new_node->data.echo_stmt.expr = expr;
	return new_node;
}

static ASTNode *parse_return() {
	ASTNode *new_node = newASTNode(NODE_RETURN);
	if (new_node == NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}
	Token *ret_k=eat();
	lexer_free_token(ret_k);
	TokenType next = peek()->type;
	if     (next != NEW_LINE &&
		next != TOKEN_SEMICOLON &&
		next != TOKEN_COMMA &&
		next != TOKEN_PIPE &&
		next != TOKEN_EOF) {
		

		ASTNode *expr = parse_expression();
		if (expr==NULL) {
			kfree(new_node);
			return NULL;
		}
		new_node->data.ret_stmt.expr=expr;
	} else {
		new_node->data.ret_stmt.expr=NULL;
	}
	return new_node;
}

static ASTNode *parse_break() {
	ASTNode *new_node = newASTNode(NODE_BREAK);
	if (new_node==NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}
	Token *break_k = eat();

	lexer_free_token(break_k);
	return new_node;
}

static ASTNode *parse_statement_list(size_t *out_s, TokenType *stop_tokens, size_t n_stop_tokens) {
	ASTNode **stmt_list = NULL;
	size_t size = 0;
	size_t cap = 4;
	stmt_list = (ASTNode**)kmalloc(sizeof(ASTNode*)*cap);
	if (stmt_list == NULL) {
		kprintf("Err -> Out of Memory\n");
		*out_s = 0;
		return NULL;
	}
	while (peek()->type != TOKEN_EOF) {
		bool is_stop=false;
		for (size_t i=0; i<n_stop_tokens;i++) {
			if (peek()->type == stop_tokens[i]) {
				is_stop=true;
				break;
			}
		}
		if (is_stop) break;

		if (peek()->type == TOKEN_NEW_LINE) {
			eat();
			continue;
		}
		ASTNode *stmt = parse_statement();
		if (stmt==NULL) {
			for (size_t i=0;i<size;i++) {
				parser_free_ast(stmt_list[i]);
			}
			kfree(stmt_list);
			*out_s = 0;
			return NULL;
		}
		while (peek()->type == TOKEN_NEW_LINE) eat(); // token_new_line
		
		if (size >= cap) {
			cap *= 2;
			ASTNode **new_stmt_list = (ASTNode**)kmalloc(sizeof(ASTNode*)*cap);
			if (new_stmt_list==NULL) {
				kprintf("Err -> Out of Memory\n");
				parser_free_ast(stmt);
				for (size_t i=0;i<size;i++) {
					parser_free_ast(stmt_list[i]);
				}
				kfree(stmt_list);
				*out_s=0;
				return NULL;
			}
			memcpy((void*)new_stmt_list, (const void*)stmt_list, size*sizeof(ASTNode*));
			kfree(stmt_list);
			stmt_list = new_stmt_list;
		}
		stmt_list[size++]=stmt;
	}
	*out_s = size;
	return stmt_list;
}

static ASTNode *parse_if() {
	ASTNode *new_node = newASTNode(NODE_IF);
	if (new_node == NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}

	Token *if_k = eat();
	lexer_free_token(if_k);
	
	new_node->data.if_stmt.condition = parse_expression();
	if (!new_node->data.if_stmt.condition) {
		kfree(new_node);
		return NULL;
	}
	while (peek()->type == TOKEN_NEW_LINE) {
		eat();
	}
	if (!match(TOKEN_THEN)) {
		kprintf("Err -> Expected 'then' after condition\n");
		parser_free_ast(new_node);
		return NULL;
	}
	while (peek()->type == TOKEN_NEW_LINE) {
		eat();
	}
	TokenType then_stop_toks[] = {TOKEN_FI, TOKEN_ELSE};

	new_node->data.if_stmt.thenb = parse_statement_list(&(new_node->data.if_stmt.s_then), then_stop_toks, 2);
	if (new_node->data.if_stmt.thenb == NULL || new_node->data.if_stmt.s_then==0) {
		parser_free_ast(new_node);
		return NULL;
	}
	if (peek()->type == TOKEN_ELSE) {
		/*
		 * SOCORROOOOOOOO
		 *
		 */
		eat();
		while (peek()->type==TOKEN_NEW_LINE) {
			eat();
		}
		TokenType else_stop_toks[] = {TOKEN_FI};
		new_node->data.if_stmt.elseb = parse_statement_list(
				&(new_node->data.if_stmt.s_else),
				else_stop_toks,
				1
				);
		if (new_node->data.if_stmt.elseb == NULL || new_node->data.if_stmt.s_else == 0) {
			parser_free_ast(new_node);
			return NULL;
		}
	} else {
		new_node->data.if_stmt.elseb = NULL;
		new_node->data.if_stmt.s_else = 0;
	}
	while (peek()->type == TOKEN_NEW_LINE) eat();

	if (!match(TOKEN_FI)) {
		kprintf("Err -> Expected 'fi'\n");
		parser_free_ast(new_node);
		return NULL;
	}
	return new_node;
}

static ASTNode *parse_while() {
	ASTNode *new_node = newASTNode(NODE_WHILE_LOOP);
	if (new_node==NULL) {
		kprintf("Err -> Out of Memory\n");
		kfree(new_node);
		return NULL;
	}
	Token *do_k = eat();
	lexer_free_token(do_k);
	
	while (peek()->type == TOKEN_NEW_LINE) eat();

	if (!match(TOKEN_LBRACE)) {
		kprintf("Err -> Expected '{' after 'do'\n");
		kfree(new_node);
		return NULL;
	}

	while (peek()->type==TOKEN_NEW_LINE) eat();

	TokenType body_stop[] = {TOKEN_RBRACE};
	new_node->data.while_loop.body = parse_statement_list(
			&(new_node->data.while_loop.s_body),
			body_stop,
			1
			);
	if (new_node->data.while_loop.body==NULL || new_node->data.while_loop.s_body == 0) {
		kprintf("Err -> Expected Statements before '}'\n");
		parser_free_ast(new_node);
		return NULL;
	}
	while (peek()->type==TOKEN_NEW_LINE) eat();

	if (!match(TOKEN_RBRACE)) {
		kprintf("Err -> Expected '}' after statements\n");
		parser_free_ast(new_node);
		return NULL;
	}
	Token *while_k = eat();
	lexer_free_token(while_k);

	new_node->data.while_loop.condition = parse_expression();
	if (new_node->data.while_loop.condition==NULL) {
		parser_free_ast(new_node);
		return NULL;
	}
	return new_node;
}

static ASTNode *parse_for() {
	ASTNode *new_node=newASTNode(NODE_FOR_LOOP);
	if (!new_node) {
		kprintf("Err -> Out of Memory\n");
		kfree(new_node);
		return NULL;
	}

	Token *for_tok=eat();
	lexer_free_token(for_tok);

	if (!match(TOKEN_LPAREN)) {
		kprintf("Err -> Expected '(' after 'for'\n");
		kfree(new_node);
		return NULL;
	}
	if (peek()->type != TOKEN_SEMICOLON) {
		new_node->data.for_loop.init = parse_expression();
		if (new_node->data.for_loop.init == NULL) {
			kfree(new_node);
			return NULL;
		}
	} else {
		new_node->data.for_loop.init = NULL;
	}
	
	if (!match(TOKEN_SEMICOLON)) {
		kprintf("Err -> Expected ';' after expression\n");
		parser_free_ast(new_node);
		return NULL;
	}

	if (peek()->type != TOKEN_SEMICOLON) {
		new_node->data.for_loop.condition = parse_expression();
		if (new_node->data.for_loop.condition==NULL) {
			parser_free_ast(new_node);
			return NULL;
		}
	} else {
		new_node->data.for_loop.condition=NULL;
	}

	if (!match(TOKEN_SEMICOLON)) {
		kprintf("Err -> Expected ';' after expression\n");
		parser_free_ast(new_node);
		return NULL;
	}

	if (peek()->type != TOKEN_RPAREN) {
		new_node->data.for_loop.inc = parse_expression();
		if (new_node->data.for_loop.inc == NULL) {
			parser_free_ast(new_node);
			return NULL;
		}
	} else {
		new_node->data.for_loop.inc = NULL;
	}

	if (!match(TOKEN_RPAREN)) {
		kprintf("Err -> Expected ')' after expression\n");
		parser_free_ast(new_node);
		return NULL;
	}

	while (peek()->type == TOKEN_NEW_LINE) eat();

	if (!match(TOKEN_DO)) {
		kprintf("Err -> Expected 'do' at the begining of a for loop\n");
		parser_free_ast(new_node);
		return NULL;
	}

	while (peek()->type == TOKEN_NEW_LINE) eat();

	TokenType body_stop[] = {TOKEN_DONE};

	new_node->data.for_loop.body = parse_statement_list(
			&(new_node->data.for_loop.s_body),
			body_stop,
			1
			);
	if (new_node->data.for_loop.body == NULL || new_node->data.for_loop.s_body == 0) {
		parser_free_ast(new_node);
		return NULL;
	}

	while (peek()->type == TOKEN_NEW_LINE) eat();

	if (!match(TOKEN_DONE)) {
		kprintf("Err -> Expected 'done' to close a for loop\n");
		parser_free_ast(new_node);
		return NULL;
	}

	return new_node;
}

static ASTNode *parse_export() {
	ASTNode *new_node = newASTNode(NODE_EXPORT);
	if (new_node==NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}
	Token *export_k = eat();
	lexer_free_token(export_k);

	if (!match(TOKEN_DOLLAR)) {
		kprintf("Err -> Expected '$' after 'export'\n");
		parser_free_ast(new_node);
		return NULL;
	}
	Token *number = eat();
	if (!match(TOKEN_COLON)) {
		kprintf("Err -> Expected ':' after var id\n");
		parser_free_ast(new_node);
		return NULL;
	}
	Token *name = eat();

	if (number->type != TOKEN_NUMBER || name->type != TOKEN_IDENTIFIER) {
		kprintf("Err -> Unexpected Tokens on a var reference\n");
		parser_free_ast(new_node);
		lexer_free_token(number);
		lexer_free_token(name);
		return NULL;
	}
	ASTNode *var_node = newASTNode(NODE_VAR_REF);
	if (var_node==NULL) {
		kprintf("Err -> Out of Memory\n");
		parser_free_ast(new_node);
		parser_free_ast(var_node);
		lexer_free_token(number);
		lexer_free_token(name);
		return NULL;
	}

	var_node->data.var_ref.num = atol(number->value);
	var_node->data.var_ref.name = strdup(name->value);
	lexer_free_token(number);
	lexer_free_token(name);
	if (var_node->data.var_ref.name == NULL) {
		kprintf("Err -> Out of Memory\n");
		parser_free_ast(new_node);
		parser_free_ast(var_node);
		return NULL;
	}

	new_node->data.export_stmt.var=var_node;
	return new_node;
}
