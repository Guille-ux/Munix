#ifndef NATIVES_H
#define NATIVES_H
#include "zynk.h"
// Add forward declarations of your native funcs here
void init_native_funcs(ArenaManager *manager, ZynkEnv *env);
Value libzynk_len(ArenaManager *manager, ZynkEnv *env, ZynkArray *args);
Value libzynk_push(ArenaManager *manager, ZynkEnv *env, ZynkArray *args);
Value libzynk_pop(ArenaManager *manager, ZynkEnv *env, ZynkArray *args);
Value libzynk_get_index(ArenaManager *manager, ZynkEnv *env, ZynkArray *args);
Value libzynk_set_index(ArenaManager *manager, ZynkEnv *env, ZynkArray *args);

#endif
