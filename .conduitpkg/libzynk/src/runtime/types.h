#ifndef ZYNK_VALUE
#define ZYNK_VALUE


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../sysarena/sysarena.h"

struct ZynkObj;
struct Value;
struct ZynkString;
struct ZynkNativeFunction;
struct ZynkArray;
struct ZynkEnv;
struct ZynkEnvTable;
struct ZynkEnvEntry;
struct ZynkFunction;

typedef enum {
  ZYNK_NULL,
  ZYNK_BOOL,
  ZYNK_NUMBER,
  ZYNK_OBJ,
  ZYNK_BYTE,
} ZYNK_TYPE;

typedef enum {
  ObjString,
  ObjNativeFunction,
  ObjFunction,
  ObjArray,
} ObjType;



typedef struct ZynkObj ZynkObj;
typedef struct Value Value;
typedef struct ZynkString ZynkString;
typedef struct ZynkNativeFunction ZynkNativeFunction;
typedef struct ZynkArray ZynkArray;
typedef struct ZynkEnv ZynkEnv;
typedef struct ZynkEnvTable ZynkEnvTable;
typedef struct ZynkEnvEntry ZynkEnvEntry;
typedef struct ZynkFunction ZynkFunction;


typedef Value (*ZynkFuncPtr)(ArenaManager *manager, ZynkEnv* env, ZynkArray* args);



struct Value {
  ZYNK_TYPE type;
  union {
    bool boolean;
    double number;
    uint8_t byte;
    ZynkObj *obj; 
  } as;
};

struct ZynkObj {
  ObjType type;
  uint32_t ref_count;
  union {
    ZynkString *string;   
    ZynkFunction *function;
    ZynkNativeFunction *native_func;
    ZynkArray *array;    
  } obj;
};

#endif // ZYNK_VALUE
