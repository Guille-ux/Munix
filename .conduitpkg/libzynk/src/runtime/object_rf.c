#include "object_rf.h"
#include "types.h"
#include "../common.h"
#include "../sysarena/sysarena.h"

Value zynk_retain(Value val) {
  if (val.type!=ZYNK_OBJ) return val;
  if (val.as.obj==NULL) return zynkNull();
  val.as.obj->ref_count++;
  return val;
}

void zynk_release(Value val, ArenaManager *manager) {
  if (val.type!=ZYNK_OBJ) return;
  if (val.as.obj==NULL) return;
  val.as.obj->ref_count--;
  if (val.as.obj->ref_count == 0) {
    // eliminar individualmente
    switch (val.as.obj->type) {
      case (ObjString): freeString(manager, val.as.obj->obj.string); break;
      case (ObjArray): freeArray(manager, val.as.obj->obj.array); break;
    }

    // eliminar el obj en su conjunto
    sysarena_free(manager, val.as.obj);
  }
}

bool freeString(ArenaManager *manager, ZynkString *string) {
  if (string==NULL) return true;
  if (!sysarena_free(manager, string->string)) return false;
  if (!sysarena_free(manager, string)) return false;

  return true;
}

bool freeArray(ArenaManager *manager, ZynkArray *array) {
  if (array==NULL) return true;
  for (size_t i=0;i<array->len;i++) {
    switch (array->array[i].type) {
      case (ZYNK_OBJ): zynk_release(array->array[i], manager); break;
    }
  }
  sysarena_free(manager, array);
  return true;
}
