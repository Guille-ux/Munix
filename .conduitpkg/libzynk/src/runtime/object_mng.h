#ifndef ZYNK_OBJECT_MANAGER
#define ZYNK_OBJECT_MANAGER

#include "../zynk.h"
#include "../common.h"
#include "types.h"
#include "objects.h"
#include "realloc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "assign.h"

Value zynkCreateNativeFunction(ArenaManager *manager, const char *name, ZynkFuncPtr func_ptr);
Value zynkCreateString(ArenaManager *manager, const char *str);
Value zynkCreateArray(ArenaManager *manager, size_t initial_capacity);
bool zynkArrayGrow(ArenaManager *manager, ZynkArray* array_ptr, uint32_t amount);
Value zynkArrayPush(ArenaManager *manager, Value array_val, Value element_val);


#endif
