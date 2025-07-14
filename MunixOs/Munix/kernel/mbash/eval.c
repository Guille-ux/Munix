#include "eval.h"

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

int eval(ASTNode *ast, EvalCtx *ctx);

static ShellValue evalExpr(ASTNode *expr, EvalCtx *ctx);
static int evalStatement(ASTNode *stmt, EvalCtx *ctx);
static int evalProgram(ASTNode *program, EvalCtx *ctx);
static ShellValue ref_add(ShellValue val);
static void ref_del(ShellValue val);

ShellValue newNumVal(long num) {
	ShellValue val;
	val.type = VAL_NUMBER;
	val.as.num = num;
	return val;
}

ShellValue newStrVal(const char *str) {
	ShellValue val;
	val.type = VAL_STRING;
	val.as.str = strdup(str);
	if (val.as.str==NULL) {
		kprintf("Err -> Out of Memory\n");
		val.type=VAL_NONE;
	}
	return val;
}

void freeVal(ShellValue val) {
	if (val.type == VAL_STRING) {
		kfree(val.as.str);
		val.as.str = NULL;
	}
}

EvalCtx *newShellCtx() {
	EvalCtx *ctx = (EvalCtx*)kmalloc(sizeof(EvalCtx));
	if (ctx==NULL) {
		kprintf("Err -> Out of Memory\n");
		return NULL;
	}
	memset((void*)ctx, 0, sizeof(EvalCtx));

	ctx->var_cap = 16;
	ctx->var_table = (Var*)kmalloc(sizeof(Var)*ctx->var_cap);
	/*
	 * Continuar
	 */
	if (ctx->var_table==NULL) {
		kprintf("Err -> Out of Memory\n");
		kfree(ctx);
		return NULL;
	}

	ctx->last_exit_code=0;
	ctx->should_break=false;
	ctx->should_ret=false;
	ctx->ret = (ShellValue){.type = VAL_NONE};
	ctx->should_exit=false;

	int exit_code = 0;
	return ctx;
}

void killShellCtx(EvalCtx *ctx) {
	if (ctx==NULL) return;

	for (size_t i=0;i<ctx->var_cap;i++) {
		if (ctx->var_table[i].is_set) {
			freeVal(ctx->var_table[i]);
		}
	}
	kfree(ctx->var_table);
	kfree(ctx);
}

Var *getShellVarEntry(EvalCtx *ctx, size_t id) {
	if (ctx->var_cap < id) {
		kprintf("Err -> Id exceeds the table capacity\n");
		return NULL;
	}
	return &ctx->var_table[id];
}

void setShellVarEntry(EvalCtx *ctx, size_t id, ShellValue val) {
	
}

static ShellValue ref_add(ShellValue val) {
	val.refc += 1;
	return val;
}

static void ref_del(ShellValue val) {
	val.refc--;
	if (val.refc<=0) {
		freeVal(val);
	}
}
