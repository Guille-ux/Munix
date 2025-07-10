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
} ASTNodeType;

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
/*
 * CONTINUAR!!!
 *
 */
} VarRefNode;
#endif
