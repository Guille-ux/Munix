#ifndef SHELL_EVAL_H
#define SHELL_EVAL_H

#include "mbtype.h"
#include "common.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "parser.h"
#include "lexer.h"

typedef enum {
	VAL_NONE,
	VAL_NUMBER,
	VAL_STRING,
} ValType;

struct Obj;
struct ShellValue;

typedef struct Obj { // para cuando use un GC
	struct ShellValue *val;
	struct Obj *next;
	bool live; // tiene o no tiene vida, ser o no ser
} Obj;

typedef struct ShellValue {
	ValType type;
	union {
		long num;
		char *str;
	} as;
	uint32_t refc; // contador de referencias, solo valido en strings
} ShellValue;

typedef struct {
	ShellValue val;
	bool is_exported;
	bool is_set;
} Var;

struct EvalCtx;

typedef struct EvalCtx {
	size_t arg_used;
	Var *var_table;
	Var *argv;
	size_t argc;
	size_t var_cap;
	
	int (*command_handler)(ASTNode *command, struct EvalCtx *ctx);

	int last_exit_code;

	bool should_break;
	bool should_ret;
	ShellValue ret;
	bool should_exit;
	
	int exit_code;
} EvalCtx;

ShellValue newNumVal(long num);
ShellValue newStrVal(const char *str);
void freeVal(ShellValue val);
void addShellArg(EvalCtx *ctx, ShellValue val);
ShellValue getShellArg(EvalCtx *ctx, size_t id);

/*
 * Status Codes
 * 
 * - 0 Ok
 * - 1 Err
 * - 2 Break Statement
 * - 3 Return Statement
 * - 4 Exit Statement (not implemented yet)
 *
 */

EvalCtx *newShellCtx();
void killShellCtx(EvalCtx *ctx);
Var *getShellVarEntry(EvalCtx *ctx, size_t id);
void setShellVarEntry(EvalCtx *ctx, size_t id, ShellValue val);
void exportShellVar(EvalCtx *ctx, size_t id);

ShellValue evalExpr(ASTNode *expr, EvalCtx *ctx);
int eval(ASTNode *ast, EvalCtx *ctx);

#endif
