#ifndef SHELL_EVAL_H
#define SHELL_EVAL_H

#include "mbtype.h"
#include "common.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
	VAL_NONE,
	VAL_NUMBER,
	VAL_STRING,
} ValType;

typedef struct {
	ValType type;
	union {
		long num;
		char *str;
	} as;
	uint32_t refc; // contador de referencias, solo valido en strings
} ShellValue;

ShellValue newNumVal(long num);
ShellValue newStrVal(const char *str);
void freeVal(ShellValue val);

typedef struct {
	ShellValue val;
	bool is_exported;
	bool is_set;
} Var;

typedef struct {
	Var *var_table;
	size_t var_cap;
	
	int last_exit_code;

	bool should_break;
	bool should_ret;
	ShellValue ret;
	bool should_exit;
	
	int exit_code;
} EvalCtx;

EvalCtx *newShellCtx();
void killShellCtx(EvalCtx *ctx);
Var *getShellVarEntry(EvalCtx *ctx, size_t id);
void setShellVarEntry(EvalCtx *ctx, size_t id, ShellValue val);
void exportShellVar(EvalCtx *ctx, size_t id);

int eval(ASTNode *ast, EvalCtx *ctx);

#endif
