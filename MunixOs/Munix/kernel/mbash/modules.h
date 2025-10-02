#include "parser.h"
#include "eval.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_MODULES 32

typedef ShellValue (*handler_t)(ASTNode *stmt, EvalCtx *ctx);

typedef struct {
  const char *name;
  handler_t module_handler;
} MBashModule_t;

static MBashModule_t modules[MAX_MODULES];
static size_t modules_count=0;

static int multimodule_handler(ASTNode *stmt, EvalCtx *ctx) {
  ShellValue ret;
  int exit_code = -1;
  for (size_t i = 0; i < modules_count; i++) {
    if (strcmp(stmt->data.command_call.name, modules[i].name)==0) {
      ret = modules[i].module_handler(stmt, ctx);
      exit_code = 0;
      break;
    }
  }
  ref_del(ret);
  setShellVarEntry(ctx, 0, ret);
  return exit_code; // no se encontro nada, F
}

static void register_module(const char *name, handler_t module) {
  if (modules_count >= MAX_MODULES) return; // no podemos añadir más módulos
  modules[modules_count].name = name; // esto esta pensado para compilation time, quiero decir, son estáticos
  modules[modules_count].module_handler = module;
  modules_count++;
}
