#include "object_mng.h"
#include "object_rf.h"
#include "realloc.h"

static ZynkObj* create_base_zynk_obj(ArenaManager* manager, ObjType type) {
    if (manager == NULL) return NULL;

    ZynkObj* obj = (ZynkObj*)sysarena_alloc(manager, sizeof(ZynkObj));
    if (obj == NULL) return NULL; // Error

    obj->type = type;
    obj->ref_count = 1;
    return obj;
}

Value zynkCreateString(ArenaManager *manager, const char *str) {
  if (manager==NULL || str==NULL) return zynkNull();
  
  size_t strlen = zynk_len(str, END_CHAR);
  
  ZynkObj *obj = create_base_zynk_obj(manager, ObjString);

  if (obj==NULL) return zynkNull();

  ZynkString *string=(ZynkString *)sysarena_alloc(manager, sizeof(ZynkString));
  if (string==NULL) {
    sysarena_free(manager, (void *)obj);
    return zynkNull();
  }

  string->string=(char *)sysarena_alloc(manager, strlen+1);
  if (string->string==NULL) {
    sysarena_free(manager, (void*)string);
    sysarena_free(manager, (void*)obj);
    return zynkNull();
  }
  zynk_cpy((uint8_t*)string->string, (uint8_t*)str, strlen);
  string->string[strlen]='\0';
  string->len=strlen;

  obj->obj.string=string;

  Value ret;
  ret.type=ZYNK_OBJ;
  ret.as.obj=obj;
  return ret;
}

Value zynkCreateNativeFunction(ArenaManager *manager, const char *name, ZynkFuncPtr func_ptr) {
  if (manager==NULL || func_ptr==NULL) {
    return zynkNull();
  }

  ZynkObj *obj=create_base_zynk_obj(manager, ObjNativeFunction);
  if (obj==NULL) {
    return zynkNull();
  }

  ZynkNativeFunction *z_func=(ZynkNativeFunction *)sysarena_alloc(manager, sizeof(ZynkNativeFunction));
  if (z_func==NULL) {
    sysarena_free(manager, (void *)obj);
    return zynkNull();
  }

  z_func->name=name;
  z_func->func_ptr=func_ptr;

  obj->obj.native_func = z_func;

  Value ret;
  ret.type = ZYNK_OBJ;
  ret.as.obj=obj;
  return ret;
}

Value zynkCreateArray(ArenaManager *manager, size_t initial_capacity) {
  if (manager==NULL) return zynkNull();
  
  if (initial_capacity==0) initial_capacity=8; // minimum capacity

  ZynkObj* obj=create_base_zynk_obj(manager, ObjArray);
  if (obj==NULL) return zynkNull();

  ZynkArray *z_arr=(ZynkArray *)sysarena_alloc(manager, sizeof(ZynkArray));
  if (z_arr==NULL) {
    sysarena_free(manager, (void *)obj);
    return zynkNull();
  }

  z_arr->array=(Value*)sysarena_alloc(manager, sizeof(Value)*initial_capacity);
  if (z_arr->array==NULL) {
    sysarena_free(manager, (void*)z_arr);
    sysarena_free(manager, (void*)obj);
    return zynkNull();
  }

  z_arr->len=0;
  z_arr->capacity=initial_capacity;

  for (size_t i=0;i<initial_capacity;i++) {
    z_arr->array[i]=zynkNull();
  }

  obj->obj.array=z_arr;

  Value ret;
  ret.type=ZYNK_OBJ;
  ret.as.obj=obj;
  return ret;
}

bool zynkArrayGrow(ArenaManager *manager, ZynkArray* array_ptr, uint32_t amount) {
  if (manager == NULL || array_ptr == NULL) return false; // validating input

  size_t old_cap=array_ptr->capacity;
  size_t new_cap=old_cap+amount; // calculating new capacity

  if (new_cap < 8) new_cap=8; // minimum capacity

  Value *new_array_data=(Value*)reallocate(
      manager,
      (uint8_t *)array_ptr->array,
      old_cap*sizeof(Value),
      new_cap*sizeof(Value)
  );

  // error?
  if (new_array_data==NULL) return false;

  array_ptr->array=new_array_data;
  array_ptr->capacity=new_cap;

  for (size_t i=old_cap;i<new_cap;i++) {
    array_ptr->array[i]=zynkNull();
  }

  return true;
}

Value zynkArrayPush(ArenaManager *manager, Value array_val, Value element_val) {
  if (array_val.type!=ZYNK_OBJ || array_val.as.obj==NULL || array_val.as.obj->type!=ObjArray) return zynkNull(); // that isn't a valid array!

  ZynkArray *arr_ptr=array_val.as.obj->obj.array;

  if (arr_ptr->len >= arr_ptr->capacity) {
    if (!zynkArrayGrow(manager, arr_ptr, (arr_ptr->len - arr_ptr->capacity + 1))) return zynkNull();
  }
  arr_ptr->array[arr_ptr->len++]=zynk_retain(element_val);
  return array_val;
}
