#ifndef BUDDY_ALLOCATOR
#define BUDDY_ALLOCATOR

#include "libcs2.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Comentado porque prefiero algo más flexible
#define BUDDY_HEAP_SIZE (1 * 1024 * 1024)
#define MIN_BLOCK_ORDER 4
#define MAX_BLOCK_ORDER 20

#if (1 << MAX_BLOCK_ORDER) != BUDDY_HEAP_SIZE
#error "BUDDY_HEAP_SIZE must be a power of 2 and equal to (1 << MAX_BLOCK_ORDER)"
#endif
#if MIN_BLOCK_ORDER >= MAX_BLOCK_ORDER
#error "MIN_BLOCK_ORDER must be less than MAX_BLOCK_ORDER"
#endif
*/

typedef struct free_node {
    struct free_node *next;
} free_node;

void buddy_init(void *heap_start, size_t heap_size, int min_block_order, free_node ***new_free_list);
void *buddy_malloc(size_t size);
void buddy_free(void *ptr);

#endif