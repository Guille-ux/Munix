#ifndef ZYNK_REALLOC_H
#define ZYNK_REALLOC_H

#include "../common.h"
#include "../sysarena/sysarena.h"
#include "../zynk.h"
#include "memory.h"
#include <stddef.h>
#include <stdint.h>

void* reallocate(ArenaManager *manager, uint8_t *pointer, size_t old_cap, size_t new_cap);

#endif
