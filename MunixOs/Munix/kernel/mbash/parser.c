/*
 * parser.c
 * sljaknsnak
 * es mi parser para mbash
 * 
 *
 *
 * No Se - Guillermo Leira Temes 2025
 *
 */

#include "parser.h"
#include "lexer.h"
#include "mbtype.h"
#include "../include/memory.h"

#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))


