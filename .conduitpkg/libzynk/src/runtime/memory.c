// zynk memory implementation
#include "memory.h"

bool zynk_cpy(uint8_t *dest, uint8_t *src, uint32_t len) {
  if (dest==NULL || src==NULL) {
    return false;
  }
  for (uint32_t i=0;i<len;i++) {
    dest[i]=src[i];
  }
  return true;
}

bool zynk_strcmp(const char *a, const char *b, uint32_t len) {
  for (uint32_t i=0;i<len;i++) {
    if (a[i]!=b[i]) {
      return false;
    }
  }
  return true;
}

uint32_t zynk_len(const char *str, char endChar) {
  if (str==NULL) {
    return 0;
  }
  for (uint32_t i=0;;i++) {
    if (str[i]==endChar) {
      return i;
    }
  }
}

bool zynkValuesEqual(Value a, Value b) {
  ZYNK_TYPE type=a.type;
  if (a.type!=b.type) return false; // they aren't the same
  switch (type) {
    case ZYNK_NULL: return true; // Null == Null
    case ZYNK_BOOL: return b.as.boolean==a.as.boolean;
    case ZYNK_NUMBER: return a.as.number==b.as.number;
    case ZYNK_BYTE: return a.as.byte==b.as.byte;
    case ZYNK_OBJ: {
                    if (a.as.obj==b.as.obj) return true; // if two objects points to the same memory area they're the same
                    ZynkObj* obj_a=a.as.obj;
                    ZynkObj* obj_b=b.as.obj;

                    ObjType obj_t=obj_a->type;
                    if (obj_t!=obj_b->type) return false; // they aren't the same type
                    switch (obj_t) {
                      case ObjString: {
                                        if (obj_a->obj.string->len!=obj_b->obj.string->len) return false;
                                        return zynk_strcmp(obj_a->obj.string->string, obj_b->obj.string->string, obj_a->obj.string->len);
                                      }
                      case ObjArray: {
                                        if (obj_a->obj.array->len!=obj_b->obj.array->len) return false;
                                        return (obj_a->obj.array->array == obj_b->obj.array->array);
                                     }
                      default: return false; // provisional
                    }
    }
    default: return false; // provisional, i'm going to change this when the system supports custom objects
  }
}

static bool isTruthy(Value val) {
  switch (val.type) {
    case ZYNK_NULL: return false;
    case ZYNK_BOOL: return val.as.boolean;
    case ZYNK_NUMBER: return val.as.number!=0;
    case ZYNK_OBJ: {
                      if (val.as.obj==NULL) return false; // null objects are false
                      return true; // objects are true
                   }
    default: return false; // Unknown types are false
  }
}

bool zynkValuesNotEqual(Value a, Value b) {
  return !zynkValuesEqual(a, b);
}

static bool areNumbers(Value a, Value b) {
  if (a.type!=ZYNK_NUMBER || b.type!=ZYNK_NUMBER) return false;
  return true;
}

bool zynkValuesLess(Value a, Value b) {
  if (!areNumbers(a, b)) return false; // wtf? this only compares numbers
  return a.as.number < b.as.number;
}

bool zynkValuesGreater(Value a, Value b) {
  if (!areNumbers(a, b)) return false; // wtf? this only compares numbers
  return a.as.number > b.as.number;
}

bool zynkValuesGreaterEqual(Value a, Value b) {
  if (!areNumbers(a, b)) return false; // wtf? this only compares numbers
  return a.as.number >= b.as.number;
}

bool zynkValuesLessEqual(Value a, Value b) {
  if (!areNumbers(a, b)) return false; // wtf? this only compares numbers
  return a.as.number <= b.as.number;
}

bool zynkValuesOr(Value a, Value b) {
  return isTruthy(a) || isTruthy(b);
}

bool zynkValuesAnd(Value a, Value b) {
  return isTruthy(a) && isTruthy(b);
}

bool zynkValuesXor(Value a, Value b) {
  return isTruthy(a) ^ isTruthy(b);
}

bool zynkValuesTrue(Value val) {
  return isTruthy(val);
}

bool zynkValuesNot(Value val) {
  return !isTruthy(val);
}

bool zynkValuesNand(Value a, Value b) {
  return !zynkValuesAnd(a, b);
}

bool zynkValuesNor(Value a, Value b) {
  return !zynkValuesOr(a, b);
}

bool zynkValuesXnor(Value a, Value b) {
  return !zynkValuesXor(a, b);
}

Value zynkValuesAdd(Value a, Value b) {
  if (!areNumbers(a, b)) return zynkNull(); // wtf?

  return zynkNumber(a.as.number + b.as.number);
}

Value zynkValuesSub(Value a, Value b) {
  if (!areNumbers(a, b)) return zynkNull(); // wtf?

  return zynkNumber(a.as.number - b.as.number);
}

Value zynkValuesMul(Value a, Value b) {
  if (!areNumbers(a, b)) return zynkNull(); // wtf?

  return zynkNumber(a.as.number * b.as.number);
}

Value zynkValuesDiv(Value a, Value b) {
  if (!areNumbers(a, b)) return zynkNull(); // wtf?

  if (a.as.number==0 || b.as.number==0) return zynkNull(); // ZeroDivisionError

  return zynkNumber(a.as.number / b.as.number);
}
