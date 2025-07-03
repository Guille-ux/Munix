#ifndef ZYNK_ENVIROMENT
#define ZYNK_ENVIROMENT


#include "../common.h"
#include "memory.h"
#include "types.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "assign.h"
#include "hash.h"
#include "../sysarena/sysarena.h"
#include "realloc.h"
#include "object_rf.h"
#include "object_mng.h"

struct ZynkEnvEntry {
  char *name;
  Value value; 
};

struct ZynkEnvTable {
  struct ZynkEnvEntry** entries;
  size_t capacity;
  size_t count;
};

struct ZynkEnv {
  struct ZynkEnvTable *local;
  struct ZynkEnv *enclosing; 
};

bool zynkEnvInit(ZynkEnv *env, size_t capacity, ZynkEnv *enclosing, ArenaManager *manager);
bool initZynkTable(ZynkEnvTable *table, size_t capacity, ArenaManager *manager);
bool freeZynkTable(ArenaManager *manager, ZynkEnvTable *table);
bool zynkTableSet(ArenaManager *manager, ZynkEnv *env, const char *str, Value value);
bool zynkTableNew(ZynkEnv *env, const char *str, Value value, ArenaManager *manager);
Value zynkTableGet(ZynkEnv *env, const char *str);
bool zynkTableDelete(ZynkEnv *env, const char *str, ArenaManager *manager);
ZynkEnvEntry* zynkFindEntry(ZynkEnv *env, const char *key, bool niu);

#endif // ZYNK_ENVIROMENT
