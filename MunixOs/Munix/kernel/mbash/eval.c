#include "eval.h"

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

// Value
static ShellValue evalNum(ASTNode *num);
static ShellValue evalStr(ASTNode *str);
static ShellValue evalVarRef(ASTNode *ref, EvalCtx *ctx);
static ShellValue evalArgRef(ASTNode *ref, EvalCtx *ctx);
static ShellValue evalBinExpr(ASTNode *expr, EvalCtx *ctx);
static ShellValue evalUnaryExpr(ASTNode *expr, EvalCtx *ctx);


// Status Code
static int evalStatement(ASTNode *stmt, EvalCtx *ctx);
static int evalProgram(ASTNode *program, EvalCtx *ctx);
static int evalCommandCall(ASTNode *call, EvalCtx *ctx);
static int evalAssignment(ASTNode *stmt, EvalCtx *ctx);
static int evalExport(ASTNode *stmt, EvalCtx *ctx);
static int evalIf(ASTNode *stmt, EvalCtx *ctx);
static int evalWhile(ASTNode *stmt, EvalCtx *ctx);
static int evalFor(ASTNode *stmt, EvalCtx *ctx);
static int evalEcho(ASTNode *stmt, EvalCtx *ctx);


static ShellValue ref_add(ShellValue val);
static void ref_del(ShellValue val);

#define EVAL_OK 0
#define EVAL_ERR 1
#define EVAL_BREAK 2
#define EVAL_RET 3
#define EVAL_EXIT 4

ShellValue newNumVal(long num) {
	ShellValue val;
	val.type = VAL_NUMBER;
	val.as.num = num;
	val.refc=0; // por si acaso
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
	val.refc=0;
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
	ctx->var_cap =16;
	ctx->argc=16;
	ctx->var_table = (Var*)kmalloc(sizeof(Var)*ctx->var_cap);
	ctx->argv = (Var*)kmalloc(sizeof(Var)*ctx->argc);
	ctx->command_handler = NULL;
	/*
	 * Continuar
	 */
	if (ctx->var_table==NULL && ctx->var_cap!=0) {
		kprintf("Err -> Out of Memory\n");
		kfree(ctx);
		return NULL;
	}

	ctx->last_exit_code=0;
	ctx->should_break=false;
	ctx->should_ret=false;
	ctx->ret = (ShellValue){.type = VAL_NONE};
	ctx->should_exit=false;
	ctx->arg_used = 0;

	int exit_code = EVAL_OK;
	return ctx;
}

void killShellCtx(EvalCtx *ctx) {
	if (ctx==NULL) return;

	for (size_t i=0;i<ctx->var_cap;i++) {
		if (ctx->var_table[i].is_set) {
			freeVal(ctx->var_table[i].val);
		}
	}
	for (size_t i=0;i<ctx->arg_used;i++) {
		freeVal(ctx->argv[i].val);
	}
	kfree(ctx->var_table);
	kfree(ctx->argv);
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
	if (id >= ctx->var_cap) {
		size_t new_cap = ctx->var_cap * 2;
		Var *new_table = (Var*)kmalloc(sizeof(Var)*new_cap);
		if (new_table==NULL) {
			kprintf("Err -> Out of Memory\n");
			return;
		}
		memcpy((void*)new_table, (const void*)ctx->var_table, ctx->var_cap);
		kfree(ctx->var_table);
		ctx->var_cap=new_cap;
		ctx->var_table = new_table;
	}
	ref_del(ctx->var_table[id].val);
	ctx->var_table[id] = (Var){.val=ref_add(val), .is_exported=false, .is_set=true};
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

void addShellArg(EvalCtx *ctx, ShellValue val) {
	if (ctx->arg_used >= ctx->argc) {
		kprintf("Err -> Too Many Args\n");
return;
	}
	ctx->argv[ctx->arg_used++] = (Var){.val=val, .is_exported=false, .is_set=true};
}

ShellValue getShellArg(EvalCtx *ctx, size_t id) {
	if (id >= ctx->arg_used) {
		kprintf("Err -> Arg n. %d doesn't exist	\n", (int)id);
		return newNumVal(0);
	}
	return ctx->argv[id].val;
}

void exportShellVar(EvalCtx *ctx, size_t id) {
	Var *var = getShellVarEntry(ctx, id);
	if (var == NULL || !var->is_set) return;
	var->is_exported=true;
}

int eval(ASTNode *ast, EvalCtx *ctx) {
	if (ctx==NULL || ast==NULL) return EVAL_ERR; // fatal error!
	
	int exit_code = evalProgram(ast, ctx);
	if (ctx->should_exit) {
		exit_code = ctx->exit_code;
	}
	return exit_code;
}

static int evalProgram(ASTNode *program, EvalCtx *ctx) {
	if (program->type!=NODE_PROGRAM) return EVAL_ERR;
	int cexit_code = EVAL_OK;
	for (size_t i=0;i<program->data.program.size;i++) {
		cexit_code = evalStatement(program->data.program.stmts[i], ctx);
		if (ctx->should_ret || ctx->should_exit) {
			break;
		}
	}
	ctx->last_exit_code = cexit_code;
	return cexit_code;
}

static int evalStatement(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt==NULL) return EVAL_ERR;
	if (ctx->should_ret) return EVAL_RET;
	if (ctx->should_exit) return EVAL_EXIT;


	switch (stmt->type) {
		case NODE_ASSIGNMENT: return evalAssignment(stmt, ctx);
		case NODE_COMMAND_CALL: return evalCommandCall(stmt, ctx);
		case NODE_EXPORT: return evalExport(stmt, ctx);
		case NODE_IF: return evalIf(stmt, ctx);
		case NODE_WHILE_LOOP: return evalWhile(stmt, ctx);
		case NODE_FOR_LOOP: return evalFor(stmt, ctx);
		case NODE_ECHO: return evalEcho(stmt, ctx);
		case NODE_BREAK: {
			ctx->should_break=true;
			return EVAL_BREAK;
				 }
		case NODE_RETURN: {
			ctx->should_ret = true;
			if (stmt->data.ret_stmt.expr!=NULL) {
				ctx->ret = evalExpr(stmt->data.ret_stmt.expr, ctx);
			} else {
				ctx->ret = newNumVal(0);
			}
			return EVAL_RET;
		default: return EVAL_ERR; // Unknown Node, FATAL ERROR!
				  }
	}
	return EVAL_OK;
}

static int evalEcho(ASTNode *stmt, EvalCtx *ctx) {
	ShellValue val = evalExpr(stmt->data.echo_stmt.expr, ctx);
	if (val.type == VAL_NUMBER) {
		kprintf("%d\n", (int)val.as.num);
	} else if (val.type == VAL_STRING) {
		kprintf("%s\n", (const char *)val.as.str);
	} else {
		kprintf("Err -> Unknown type\n");
	}
	ref_del(val);
	return EVAL_OK;
}

static int evalAssignment(ASTNode *stmt, EvalCtx *ctx) {
	ShellValue val = evalExpr(stmt->data.assignment.expr, ctx);
	size_t id = stmt->data.assignment.num;
	setShellVarEntry(ctx, id, val);
	return EVAL_OK;
}

static int evalExport(ASTNode *stmt, EvalCtx *ctx) {
	ASTNode *var = stmt->data.export_stmt.var;
	if (var==NULL) {
		return EVAL_ERR;
	}

	size_t id=var->data.var_ref.num;
	exportShellVar(ctx, id);
	return EVAL_OK;
}

static bool isTruly(ShellValue val) {
	if (val.type == VAL_NUMBER) {
		return (val.as.num != 0);
	} else if (val.type == VAL_STRING) {
		return (val.as.str != NULL && strlen(val.as.str) > 0);
	} else {
		return false;
	}
}

static int evalIf(ASTNode *stmt, EvalCtx *ctx) {
	ShellValue cond_val = evalExpr(stmt->data.if_stmt.condition, ctx);

	bool is_true = isTruly(cond_val);
	freeVal(cond_val);

	int status = EVAL_OK;
	if (is_true) {
		for (size_t i=0;i<stmt->data.if_stmt.s_then;i++) {
			status = evalStatement(stmt->data.if_stmt.thenb[i], ctx);
			if (status == EVAL_BREAK || status == EVAL_ERR || status == EVAL_RET || EVAL_EXIT) return status;
		}
	} else if (stmt->data.if_stmt.elseb!=NULL) {
		for (size_t i=0;i<stmt->data.if_stmt.s_else;i++) {
			status = evalStatement(stmt->data.if_stmt.elseb[i], ctx);
			if (status==EVAL_BREAK || status==EVAL_RET || status == EVAL_EXIT || status == EVAL_ERR) return status;
		}
	}
	return status;
}

static int evalWhile(ASTNode *stmt, EvalCtx *ctx) {
	int status = EVAL_OK;
	do {
		for (size_t i=0;i<stmt->data.while_loop.s_body;i++) {
			status = evalStatement(stmt->data.while_loop.body[i], ctx);
			if (status == EVAL_BREAK) {
				ctx->should_break = false;
				return EVAL_OK;
			}
			if (status == EVAL_ERR || status == EVAL_EXIT || status == EVAL_RET) return status;
		}

		ShellValue cond_val=evalExpr(stmt->data.while_loop.condition, ctx);
		bool keep = isTruly(cond_val);
		freeVal(cond_val);
		if (!keep) break;
	} while (true);
	return EVAL_OK;
}

static int evalFor(ASTNode *stmt, EvalCtx *ctx) {
	int status=EVAL_OK;

	if (stmt->data.for_loop.init) {
		evalStatement(stmt->data.for_loop.init, ctx);
	}
	while (true) {
		bool keep=false;
		if (stmt->data.for_loop.condition) {
			ShellValue cond_val = evalExpr(stmt->data.for_loop.condition, ctx);
			keep = isTruly(cond_val);
			freeVal(cond_val);
		}
		if (!keep) break;

		for (size_t i=0;i<stmt->data.for_loop.s_body;i++) {
			status = evalStatement(stmt->data.for_loop.body[i], ctx);
			if (status == EVAL_BREAK) {
				ctx->should_break=false;
				return EVAL_OK;
			}
			if (status != EVAL_OK) return status; // me di cuenta de que esto es más rápido, xD
		}
		if (stmt->data.for_loop.inc) {
			evalStatement(stmt->data.for_loop.inc, ctx);
		}
	}
	return EVAL_OK;
}

ShellValue evalExpr(ASTNode *expr, EvalCtx *ctx) {
	if (!expr || !ctx) {
		kprintf("Err -> Invalid parameters\n");
		return (ShellValue){.type=VAL_NONE};
	}
	switch (expr->type) {
		case NODE_NUM_LIT: return evalNum(expr);
		case NODE_STRING_LIT: return evalStr(expr);
		case NODE_VAR_REF: return evalVarRef(expr, ctx);
		case NODE_ARG_REF: return evalArgRef(expr, ctx);
		case NODE_BINARY_EXPR: return evalBinExpr(expr, ctx);
		case NODE_UNARY: return evalUnaryExpr(expr, ctx);
		default: kprintf("Err -> Unknown Op\n"); return (ShellValue){.type=VAL_NONE};
	}
}

static ShellValue evalNum(ASTNode *num) {
	return newNumVal(num->data.number_lit);
}

static ShellValue evalStr(ASTNode *str) {
	return newStrVal((const char*)str->data.string_lit);
}

static ShellValue evalVarRef(ASTNode *ref, EvalCtx *ctx) {
	Var *var_entry = getShellVarEntry(ctx, ref->data.var_ref.num);
	if (var_entry==NULL) {
		return (ShellValue){.type=VAL_NONE};
	}
	return var_entry->val;
}

static ShellValue evalArgRef(ASTNode *ref, EvalCtx *ctx) {
	return getShellArg(ctx, ref->data.arg_ref.num);
}

static ShellValue evalUnaryExpr(ASTNode *expr, EvalCtx *ctx) {
	ShellValue expected = evalExpr(expr->data.unary_expr.expr, ctx); // no se porque se llama expected
	if (expected.type==VAL_NONE) {
		return expected;
	}
	ShellValue result;
	switch (expr->data.unary_expr.op) {
		case TOKEN_MINUS: {
			if (expected.type == VAL_NUMBER) {
				result = newNumVal(-expected.as.num);
			} else {
				return expected;
			}
			break;
				  }
		case TOKEN_BANG: {
			result = (ShellValue){.type=VAL_NUMBER, .as.num=(long)(!isTruly(expected))};
			break;
				 }
		default: return expected;
	}
	ref_del(expected);
	return result;
}

static ShellValue evalBinExpr(ASTNode *expr, EvalCtx *ctx) {
	ShellValue left = evalExpr(expr->data.binary_expr.left, ctx);
	if (left.type==VAL_NONE) {
		return left; // por pereza a crear un nuevo objeto
	}
	ShellValue right = evalExpr(expr->data.binary_expr.right, ctx);
	if (right.type==VAL_NONE) {
		return right; // por pereza
	}

	ShellValue result;

	bool is_math = (expr->data.binary_expr.op == TOKEN_PLUS ||
                             expr->data.binary_expr.op == TOKEN_MINUS ||
                             expr->data.binary_expr.op == TOKEN_MUL ||
                             expr->data.binary_expr.op == TOKEN_DIV);
	bool is_cmp = (expr->data.binary_expr.op == TOKEN_EQ ||
                             expr->data.binary_expr.op == TOKEN_GREATER ||
                             expr->data.binary_expr.op == TOKEN_LESS ||
                             expr->data.binary_expr.op == TOKEN_LEQ ||
                             expr->data.binary_expr.op == TOKEN_GEQ ||
                             expr->data.binary_expr.op == TOKEN_BEQ);

	if (is_math) {
		/*
		 * En el futuro mejorare esto, digo, añadire coerciones
		 * no se
		 *
		 */
		if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
			switch (expr->data.binary_expr.op) {
				case TOKEN_MINUS: result = newNumVal(left.as.num - right.as.num); break;
				case TOKEN_MUL: result = newNumVal(left.as.num * right.as.num); break;
				case TOKEN_DIV: {
					if (right.as.num==0) {
						result = newNumVal(0);
					} else {
						result = newNumVal(left.as.num / right.as.num);
					}
					break;
						}
				case TOKEN_PLUS: result = newNumVal(left.as.num + right.as.num); break;
			}
		} else if (left.type == VAL_STRING || right.type == VAL_STRING) {
			if (expr->data.binary_expr.op==TOKEN_PLUS) {
				char *right_str;
				char *left_str;
				char num_buff[66];
				if (left.type == VAL_STRING) {
					left_str = left.as.str;
				} else {
					ltoa(left.as.num, num_buff, 10);
					left_str = num_buff;
	
				} if (right.type == VAL_STRING) {
					right_str = right.as.str;
				} else {
					ltoa(right.as.num, num_buff, 10);
					right_str = num_buff;
				}
				char *concat = (char*)kmalloc(strlen(left_str)+strlen(right_str)+1);
				if (concat==NULL) {
					kprintf("[FATAL ERROR]\n");
					return result;
				}
				strcpy(concat, left_str);
				strcat(concat, right_str);
	
				result = (ShellValue){.type=VAL_STRING, .as.str=concat, .refc=0};
			}
		}
	} else if (is_cmp) {
		long num_left=0, num_right=0;
		char *str_left=NULL, *str_right=NULL;
		bool can_num=false;
		bool can_str=false;

		if (left.type==VAL_NUMBER) {
			num_left = left.as.num;
			can_num=true;
		} else if (left.type==VAL_STRING && left.as.str!=NULL) {
			str_left = left.as.str;
			can_str = true;
		} else {
			kprintf("Err -> Illegal type\n");
			result = newNumVal(0);
		}
		if (right.type==VAL_NUMBER) {
			num_right = right.as.num;
			can_num = can_num && true;
		} else if (right.type==VAL_STRING && right.as.str!=NULL) {
			str_right = right.as.str;
			can_str = can_str && true;
		} else {
			kprintf("Err -> Illegal type\n");
			result = newNumVal(0);
		}

		if (can_num) {
			switch (expr->data.binary_expr.op) {
				case TOKEN_OR: result=newNumVal(num_left || num_right); break;
				case TOKEN_AND: result=newNumVal(num_left && num_right); break;
				case TOKEN_EQ: result=newNumVal(num_left==num_right); break;
				case TOKEN_GREATER: result=newNumVal(num_left>num_right); break;
				case TOKEN_LESS: result=newNumVal(num_left < num_right); break;
				case TOKEN_LEQ: result=newNumVal(num_left <= num_right); break;
				case TOKEN_GEQ: result=newNumVal(num_left >= num_right); break;
				case TOKEN_BEQ: result=newNumVal(num_left != num_right); break;
			}
		} else if (can_str) {
			int cmp = strcmp(str_left, str_right);
			switch (expr->data.binary_expr.op) {
				case TOKEN_EQ: result=newNumVal(cmp==0); break;
				case TOKEN_BEQ: result=newNumVal(cmp!=0); break;
				case TOKEN_LEQ: result=newNumVal(cmp<=0); break;
				case TOKEN_GEQ: result=newNumVal(cmp>=0); break;
				case TOKEN_LESS: result=newNumVal(cmp<0); break;
				case TOKEN_GREATER: result=newNumVal(cmp>0); break; 
			}
		} else {
			kprintf("Err -> Unknown Operation\n");
			result = (ShellValue){.type=VAL_NONE};
		}
	}

	ref_add(left);
	ref_del(right);
	return result;
}

static int evalCommandCall(ASTNode *stmt, EvalCtx *ctx) {
	if (ctx->command_handler == NULL) {
		return EVAL_OK; // si no hay nada, devolvemos ok y a llorar
	} else {
		return ctx->command_handler(stmt, ctx);
	}
}
