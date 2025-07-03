#include "types.h"
#include "objects.h"
#include "assign.h"
#include "../common.h"
#include "zynk_enviroment.h"
#include "realloc.h"
#include "object_mng.h"
#include "object_rf.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define IS_OBJ(val) (val.type==ZYNK_OBJ)

Value zynkArrayGet(Value array_val, Value index_val) {
  if (!IS_OBJ(array_val) || array_val.as.obj->type!=ObjArray) return zynkNull();

  ZynkArray* array_obj = array_val.as.obj->obj.array;

  if (index_val.type != ZYNK_NUMBER) return zynkNull();

  uint32_t index=index_val.as.number;

  index = (array_obj->len + index) % array_obj->len; // solucionar problemas de out-of-bounds
  return array_obj->array[index];
}

void zynkArraySet(ArenaManager *manager, Value array_val, Value index_val, Value new_element) {
  if (!IS_OBJ(array_val) || array_val.as.obj->type!=ObjArray) return;

  ZynkArray* array_obj = array_val.as.obj->obj.array;

  if (index_val.type != ZYNK_NUMBER) return;

  uint32_t index=index_val.as.number;

  index = (array_obj->len + index) % array_obj->len;

  zynk_release(array_obj->array[index], manager);

  array_obj->array[index] = zynk_retain(new_element);
}

Value zynkArrayPop(ArenaManager *manager, Value array_val) {
  if (!IS_OBJ(array_val) || array_val.as.obj->type!=ObjArray) return zynkNull();
  ZynkArray* array_obj = array_val.as.obj->obj.array;
  if (array_obj->len==0) return zynkNull();
  array_obj->len--;
  Value popped=array_obj->array[array_obj->len];
  array_obj->array=(Value *)reallocate(manager, (uint8_t *)array_obj->array, sizeof(Value)*array_obj->capacity, array_obj->len*sizeof(Value));
  return popped;
}

#undef IS_OBJ
