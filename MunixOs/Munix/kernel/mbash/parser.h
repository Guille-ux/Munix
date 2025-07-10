#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "mbtype.h"
#include "common.h"
#include "../include/ksysarena.h"
#include "../include/memory.h"

typedef enum {
	NODE_PROGRAM,
	NODE_ASSIGNMENT,
	NODE_COMMAND_CALL,
	NODE_EXPORT,
	NODE_VAR_REF,
	NODE_STRING_LIT,
	NODE_NUM_LIT,
	NODE_BINARY_EXPR,
	NODE_IF,
	NODE_WHILE_LOOP,
	NODE_FOR_LOOP,
	NODE_BREAK,
	NODE_ECHO,
	NODE_RETURN,
	NODE_UNARY,
} ASTNodeType;

struct BreakNode;
struct ASTNode;
struct ProgramNode;
struct AssignmentNode;
struct CommandCallNode;
struct VarRefNode;
struct ExportNode;
struct BinExprNode;
struct IfNode;
struct WhileNode;
struct ForNode;
struct EchoNode;
struct RetNode;
struct UnaryNode;

typedef struct ProgramNode {
	struct ASTNode **stmts;
	size_t size;
} ProgramNode;

typedef struct AssignmentNode {
	char *var_name;
	struct ASTNode *expr;
} AssignmentNode;

typedef struct CommandCallNode {
	char *name;
	struct ASTNode **args;
	size_t argc;
} CommandCallNode;

typedef struct VarRefNode {
	char *name;
} VarRefNode;

typedef struct BinExprNode {
	TokenType op;
	struct ASTNode *left;
	struct ASTNode *right;
} BinExprNode;

typedef struct IfNode {
	struct ASTNode *condition;
	struct ASTNode **thenb;
	struct ASTNode **elseb;
	size_t s_then;
	size_t s_else;
} IfNode;

typedef struct WhileNode {
	struct ASTNode *condition;
	struct ASTNode **body;
	size_t s_body;
} WhileNode;

typedef struct ForNode {
	struct ASTNode *init;
	struct ASTNode *condition;
	struct ASTNode *inc;
	struct ASTNode **body;
	size_t s_body;
} ForNode;

typedef struct EchoNode {
	struct ASTNode *expr;
} EchoNode;

typedef struct RetNode {
	struct ASTNode *expr;
} RetNode;

typedef struct UnaryNode {
	TokenType op;
	struct ASTNode *expr;
} UnaryNode;

typedef struct ASTNode {
	ASTNodeType type;
	union {
		ProgramNode program;
		AssignmentNode assignment;
		CommandCallNode command_call;
		VarRefNode var_ref;
		BinExprNode binary_expr;
		IfNode if_stmt;
		WhileNode while_loop;
		ForNode for_loop;
		EchoNode echo_stmt;
		RetNode ret_stmt;
		UnaryNode unary_expr;

		char *string_lit;
		long number_lit;
	} data;
} ASTNode;

#endif
