#include "hash.h"

uint32_t zynk_hash_string(const char *str) {
  unsigned long hash=5381;
  int c;
  while ((c=*str++)) {
    // original = (hash<<5)+hash+c
    // second = ((hash<<5)+hash)^c
    // third = ((hash<<5)+hash)^((c<<5)+c)
    // forth = ((hash<<5)+hash)^((c<<6)+c)
    // fifth (test) = ((hash<<5)+hash)^(c*c)
    // another test =  ((hash << 5) + hash) ^ (c ^ (c << 8) ^ (c << 16) ^ (c << 24))
    hash = ((hash << 5) + hash) ^ (c ^ (c << 8) ^ (c << 16) ^ (c << 24));
  }
  return hash;
}
