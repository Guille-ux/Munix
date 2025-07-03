#include "realloc.h"
#include "memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void* reallocate(ArenaManager *manager, uint8_t *pointer, size_t old_cap, size_t new_cap) {
  if (new_cap==0) sysarena_free(manager, pointer); return NULL;
  if (old_cap==0) return sysarena_alloc(manager, new_cap);
  if (old_cap==new_cap) return pointer;
  
  uint8_t *new_ptr=sysarena_alloc(manager, new_cap);
  if (new_ptr==NULL) return NULL;
  if (old_cap<new_cap) {
    zynk_cpy(new_ptr, pointer, old_cap);
  } else {
    zynk_cpy(new_ptr, pointer, new_cap);
  }
  sysarena_free(manager, pointer);
  return new_ptr;
}
