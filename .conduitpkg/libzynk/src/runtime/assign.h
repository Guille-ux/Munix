#ifndef ZYNK_ASSIGN
#define ZYNK_ASSIGN

#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

Value zynkNull();
Value zynkBool(bool tf);
Value zynkNumber(double number);

#endif
