#include "../natives.h"
#include "../zynk.h"
#include <stdbool.h>

#ifndef IS_OBJ
#define IS_OBJ(val) (val.type==ZYNK_OBJ)
#endif

void init_native_funcs(ArenaManager *manager, ZynkEnv *env) {
  zynkTableNew(env, "len", zynkCreateNativeFunction(manager, "__len__", (ZynkFuncPtr)libzynk_len), manager);
  zynkTableNew(env, "push", zynkCreateNativeFunction(manager, "__push__", (ZynkFuncPtr)libzynk_push), manager);
  zynkTableNew(env, "pop", zynkCreateNativeFunction(manager, "__pop__", (ZynkFuncPtr)libzynk_pop), manager);
  zynkTableNew(env, "get_index", zynkCreateNativeFunction(manager, "__get_index__", (ZynkFuncPtr)libzynk_get_index), manager);
  zynkTableNew(env, "set_index", zynkCreateNativeFunction(manager, "__set_index__", (ZynkFuncPtr)libzynk_set_index), manager);
}

Value libzynk_len(ArenaManager *manager, ZynkEnv *env, ZynkArray *args) {
  if (manager == NULL || env==NULL) return zynkNull();

  Value obj = args->array[0];

  if (!IS_OBJ(obj) || obj.as.obj==NULL) return zynkNull();

  switch (obj.as.obj->type) {
    case ObjString: return zynkNumber(obj.as.obj->obj.string->len);
    case ObjArray: return zynkNumber(obj.as.obj->obj.array->len);
    default: return zynkNull();
  }
}

Value libzynk_push(ArenaManager *manager, ZynkEnv *env, ZynkArray *args) {
  if (manager==NULL || env==NULL) return zynkBool(false);

  Value obj = args->array[0];
  Value new_element = args->array[1];

  if (!IS_OBJ(obj) || obj.as.obj==NULL) return zynkBool(false);

  switch (obj.as.obj->type) {
    case ObjString: {
      if (!IS_OBJ(new_element)) return zynkBool(false);
      char *ptr=obj.as.obj->obj.string->string;
      uint32_t new_cap = obj.as.obj->obj.string->len+2; // hay que tener en cuenta '\0'
      ptr=(char*)reallocate(manager, (uint8_t*)ptr, obj.as.obj->obj.string->len+1, new_cap);
      if (ptr==NULL) return zynkBool(false);
      ptr[new_cap-2]=(char)new_element.as.obj->obj.string->string[0];
      ptr[new_cap-1]='\0';
      obj.as.obj->obj.string->string=ptr;
      return zynkBool(true);
                    }
    case ObjArray: {
                zynkArrayPush(manager, obj, new_element);
                return zynkBool(true);
                   }
    default: return zynkBool(false);
  }
}

Value libzynk_pop(ArenaManager *manager, ZynkEnv *env, ZynkArray *args) {
  if (manager==NULL || env==NULL) return zynkNull();

  Value obj = args->array[0];

  if (!IS_OBJ(obj) || obj.as.obj==NULL) return zynkNull();

  switch (obj.as.obj->type) {
    case ObjString: {
      if (obj.as.obj->obj.string->len<=0) return zynkNull();
      uint32_t len=obj.as.obj->obj.string->len;
      char *ptr=obj.as.obj->obj.string->string;
      char result=ptr[len-1];
      ptr=(char*)reallocate(manager, (uint8_t*)ptr, len+1, len);
      if (ptr==NULL) return zynkNull();
      ptr[len-1]='\0';
      obj.as.obj->obj.string->string=ptr;
      obj.as.obj->obj.string->len--;
      char buff[2];
      buff[0]=result;
      buff[1]='\0';
      return zynkCreateString(manager, (const char*)buff);


                    }
    case ObjArray: {
      return zynkArrayPop(manager, obj);
                   }
    default: return zynkNull();
  }
}

Value libzynk_get_index(ArenaManager *manager, ZynkEnv *env, ZynkArray *args) {
  if (manager==NULL || env==NULL) return zynkNull();

  Value obj = args->array[0];
  uint32_t index = args->array[1].as.number;
  if (!IS_OBJ(obj) || obj.as.obj==NULL) return zynkNull();
  
  switch (obj.as.obj->type) {
    case ObjString: {
      uint32_t len=obj.as.obj->obj.string->len;
      if (!(len <= index)) return zynkNull();
      char buff[2];
      buff[0]=obj.as.obj->obj.string->string[index];
      buff[1]='\0';
      return zynkCreateString(manager, (const char*)buff);
                    }
    case ObjArray: {
      return zynkArrayGet(obj, args->array[1]);
                   }
    default: return zynkNull();
  }
}

Value libzynk_set_index(ArenaManager *manager, ZynkEnv *env, ZynkArray *args) {
  if (manager==NULL || env==NULL) return zynkNull();

  Value obj = args->array[0];
  uint32_t index = args->array[1].as.number;
  Value new_element = args->array[2];
  if (!IS_OBJ(obj) || obj.as.obj==NULL) return zynkBool(false);
  switch (obj.as.obj->type) {
    case ObjString: {
      uint32_t len=obj.as.obj->obj.string->len;
      if (!(len <= index)) return zynkBool(false);
      if (!(new_element.as.obj->type==ObjString)) return zynkBool(false);
      obj.as.obj->obj.string->string[index]=new_element.as.obj->obj.string->string[0];
      return zynkBool(true);
                    }
    case ObjArray: {
      zynkArraySet(manager, obj, args->array[1], new_element);
      return zynkBool(true);
                   }
  }
}


#undef IS_OBJ
