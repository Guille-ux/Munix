#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../common.h"
#include "calls.h"
#include "types.h"

#define IS_NULL(obj) (obj.type==ZYNK_NULL)
#define IS_OBJ(val) (val.type==ZYNK_OBJ)
#define AS_OBJ(val) (val.as.obj)
#define IS_ARRAY(obj) (obj->type==ObjArray)

Value zynkCallFunction(ArenaManager *manager, ZynkEnv *env, const char *name, Value args) {
  Value func = zynkTableGet(env, name);

  if (!IS_OBJ(args) || !IS_OBJ(func) || !IS_ARRAY(AS_OBJ(args))) return zynkNull();

  ZynkArray *array_obj=AS_OBJ(args)->obj.array;

  switch (AS_OBJ(func)->type) {
    case ObjNativeFunction: return AS_OBJ(func)->obj.native_func->func_ptr(manager, env, array_obj);
    case ObjFunction: return zynkNull(); // future...
    default: return zynkNull();
  }
}

#undef IS_NULL
#undef IS_OBJ
#undef AS_OBJ
#undef IS_ARRAY
