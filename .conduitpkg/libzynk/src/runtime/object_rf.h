#ifndef ZYNK_REFERENCE_COUNTER
#define ZYNK_REFERENCE_COUNTER

#include "types.h"
#include "../common.h"
#include "../sysarena/sysarena.h"

Value zynk_retain(Value val);
void zynk_release(Value val, ArenaManager *manager);
bool freeString(ArenaManager *manager, ZynkString* string);
bool freeArray(ArenaManager *manager, ZynkArray* array);

#endif
