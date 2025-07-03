#ifndef ZYNK_MEMORY
#define ZYNK_MEMORY

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "types.h"
#include "objects.h"
#include "object_rf.h"
#include "object_mng.h"
#include "../common.h"
#include "assign.h"

bool zynk_cpy(uint8_t *dest, uint8_t *src, uint32_t len);
bool zynk_strcmp(const char *a, const char *b, uint32_t len);
uint32_t zynk_len(const char *str, char endChar);
// bool zynk_arrcmp(ZynkArray *a, ZynkArray *b); // future

// All Types
bool zynkValuesEqual(Value a, Value b);
bool zynkValuesNotEqual(Value a, Value b);

// Numbers
bool zynkValuesLess(Value a, Value b);
bool zynkValuesGreater(Value a, Value b);
bool zynkValuesGreaterEqual(Value a, Value b);
bool zynkValuesLessEqual(Value a, Value b);

// Booleans
bool zynkValuesOr(Value a, Value b);
bool zynkValuesAnd(Value a, Value b);
bool zynkValuesXor(Value a, Value b);
bool zynkValuesNot(Value val);
bool zynkValuesNand(Value a, Value b);
bool zynkValuesNor(Value a, Value b);
bool zynkValuesXnor(Value a, Value b);
bool zynkValuesTrue(Value val);

// Math
Value zynkValuesAdd(Value a, Value b);
Value zynkValuesSub(Value a, Value b);
Value zynkValuesMul(Value a, Value b);
Value zynkValuesDiv(Value a, Value b);
#endif
