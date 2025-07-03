// Code under LGPL
// Copyright 2025 Guillermo Leira Temes

#include "zynk_enviroment.h"
#include "../common.h"
#include "hash.h"
#include "memory.h"
#include "types.h"
#include "assign.h"
#include "../sysarena/sysarena.h"

bool zynkEnvInit(ZynkEnv *env, size_t capacity, ZynkEnv *enclosing, ArenaManager *manager) {
  if (env==NULL) {
    return false; //no existe xD
  }
  env->enclosing=enclosing;
  return initZynkTable(env->local, capacity, manager);
}

bool initZynkTable(ZynkEnvTable *table, size_t capacity, ArenaManager *manager) {
  if (table==NULL || table->entries==NULL) {
    return false; // no existe xD
  }
  table->capacity=capacity;
  for (size_t i=0;i<capacity;i++) {
    if (table->entries[i]==NULL){
      table->entries[i]=(ZynkEnvEntry *)sysarena_alloc(manager, sizeof(ZynkEnvEntry));
    }
    table->entries[i]->name=NULL;
    table->entries[i]->value=zynkNull();
  }
  return true;
}
bool freeZynkTable(ArenaManager *manager, ZynkEnvTable *table) {
  if (table==NULL || table->entries==NULL || table->capacity==0 || manager==NULL) {
    return false;
  } 
  for (size_t i=0;i<table->capacity;i++) {
    if (table->entries[i]==NULL) {
      continue;
    }
    if (table->entries[i]==NULL) continue;
    if (table->entries[i]->name==NULL) continue;
    if (sysarena_free(manager, table->entries[i]->name)==false){
      return false;
    }
    if (sysarena_free(manager, table->entries[i])==false){
      return false;
    }
    table->entries[i]->name=NULL;
    table->entries[i]=NULL;
  }
  bool result=(sysarena_free(manager, table->entries) && sysarena_free(manager, table));
  table->entries=NULL;
  table=NULL;
  return result;
}
bool zynkTableSet(ArenaManager *manager, ZynkEnv *env, const char *str, Value value) {
  if (env==NULL || str==NULL || env->local->capacity==0 || env->local==NULL) {
    return false;
  }
  ZynkEnvEntry *entry=zynkFindEntry(env, str, false);
  if (entry==NULL) {
    return false;
  }

  zynk_release(entry->value, manager);

  zynk_retain(value);

  entry->value=value;
  return true;
}
bool zynkTableNew(ZynkEnv *env, const char *str, Value value, ArenaManager *manager) {
  if (env==NULL || str==NULL || env->local->capacity==0 || env->local->count == env->local->capacity || env->local==NULL) {
    return false;
  }
  ZynkEnvEntry *entry=zynkFindEntry(env, str, true);
  if (entry==NULL) {
    return false;
  } else if (entry->name==NULL) {
    char *name = (char *)sysarena_alloc(manager, zynk_len(str, END_CHAR)+1);
    zynk_retain(value);
    entry->value = value;
    zynk_cpy(name, str, zynk_len(str, END_CHAR)+1);
    entry->name=name;
  } else {
    return false;
  }
  env->local->count++;
  return true;
}
Value zynkTableGet(ZynkEnv *env, const char *str) {
  if (env==NULL || str==NULL || env->local->capacity==0 || env->local==NULL) {
    return zynkNull();
  }
  ZynkEnvEntry *entry=zynkFindEntry(env, str, false);
  if (entry==NULL) {
    return zynkNull();
  }
  return entry->value;
}
bool zynkTableDelete(ZynkEnv *env, const char *str, ArenaManager *manager) {
  if (env==NULL || str==NULL || env->local->capacity==0 || env->local==NULL || env->local->entries==NULL) {
    return false;
  }
  ZynkEnvEntry *entry=zynkFindEntry(env, str, false);
  if (entry==NULL || entry->name == NULL) {
    return false;
  }
  sysarena_free(manager, entry->name);
  entry->name=NULL;
  zynk_release(entry->value, manager);
  entry->value=zynkNull();
  env->local->count--;
  return true;
}
ZynkEnvEntry *zynkFindEntry(ZynkEnv *env, const char *key, bool niu) {
  size_t capacity=env->local->capacity;
  uint32_t index = zynk_hash_string(key) % capacity;
  uint32_t a = zynk_len(key, END_CHAR);
  uint32_t limit=index;
  for (;;) {
    ZynkEnvEntry *entry= env->local->entries[index];
    if (entry==NULL) {
      return NULL;
    } else if ((entry->name==NULL && niu) || (a==zynk_len(entry->name, END_CHAR) && zynk_strcmp(entry->name, key, a))) {
      return entry;
    }
    index = (index+1) % capacity;
    if (index==limit) {
      return NULL;
    }
  }
  if (env->enclosing==NULL) {
    return NULL;
  }
  return zynkFindEntry(env->enclosing, key, false);
}
