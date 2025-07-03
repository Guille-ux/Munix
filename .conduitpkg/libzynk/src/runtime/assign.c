#include "types.h"
#include "assign.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../common.h"

inline void initVal(Value *val) {
  // something here to initialize values
}

Value zynkNull() {
  Value ret;
  ret.type=ZYNK_NULL;
  initVal(&ret); 
  return ret;
}

Value zynkBool(bool tf) {
  Value ret;
  ret.type=ZYNK_BOOL;
  ret.as.boolean=tf;
  initVal(&ret);
  return ret;
}

Value zynkNumber(double number) {
  Value ret;
  ret.type=ZYNK_NUMBER;
  ret.as.number=number;
  initVal(&ret);
  return ret;
}
