#include "../include/buddy_alloc.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static char *heap_base=NULL;
static size_t heap_total_size=0;
static int max_order=0;
static int min_order=0;
static int num_orders=0;

static free_node **free_list=NULL;

typedef struct block_header {
    int order;
} block_header;


/* Funciones Auxiliares */
static int log2_int(size_t n) {
    if (n==0) return -1; // error
    int count=0;
    while (n>1) {
        n >>= 1;
        count++;
    }
    return count;
}



static int size2order(size_t size) {
    if (size == 0) return -1;
        
    size_t requested_total_size = size + sizeof(block_header);
        
    if (requested_total_size <= (1 << min_order)) {
        return min_order;
    }
        
    int order = log2_int(requested_total_size);
    if ((1 << order) < requested_total_size) {
        order++;
    }
        
    if (order > max_order) return -1;
        
    return order;
}

void buddy_init(void *heap_start, size_t heap_size, int min_block_order, free_node ***new_free_list) {
    if (heap_start == NULL || heap_size == 0 || (heap_size & (heap_size - 1)) != 0) {
        kprintf("Err -> Invalid heap parameters\n");
        return;
    }
    heap_base = (char*)heap_start;
    heap_total_size = heap_size;

    max_order = log2_int(heap_size);
    if (max_order == -1 || max_order < min_block_order) {
        kprintf("Err -> heap size %d is to small\n", (int)heap_size);
        heap_base = NULL;
        return;
    }
    min_order = min_block_order;
    num_orders = max_order - min_order + 1;
    /* Esto solo se uso para solucionar errores
    kprintf("Num Orders: %d\n", (int)num_orders);
    kprintf("Max Order: %d\n", (int)max_order);
    kprintf("Min Order: %d\n", (int)min_order);   
    kprintf("Available Memory: %d\n", (int)heap_size);
    */
    free_list = *new_free_list;
    for (int i=0;i<num_orders;i++) {
        free_list[i]=NULL;
    }

    free_node *initial_block = (free_node *)heap_base;
    initial_block->next = NULL;

    int initial_idx = max_order - min_order;
    free_list[initial_idx]=initial_block;
    kprintf("Buddy allocator initialized!\n");
}

void *buddy_malloc(size_t size) {
    if (size == 0) return NULL;
    size_t alloc_size = size + sizeof(block_header);

    int target_order = size2order(alloc_size);

    if (target_order == -1 || target_order > max_order) {
        kprintf("Err -> Invalid Order\n");
        return NULL;
    }

    int clist_idx = target_order - min_order;
    int found_order = -1; // -1 por defector por si hay un error
    for (int i=clist_idx;i<num_orders;i++) {
        if (free_list[i]!=NULL) {
            found_order = i;
            break;
        }
    }

    if (found_order == -1) { // si no hay memoria
        kprintf("Malloc -> Out of Memory\n");
        return NULL;
    }

    free_node *to_alloc=free_list[found_order];
    free_list[found_order]=to_alloc->next;

    int real_order = found_order + min_order;

    while (real_order > target_order) {
        real_order--;
        size_t new_size = (1 << real_order);
        
        void *new_buddy = (void*)((char*)to_alloc + new_size);
        
        free_node *buddy_node=(free_node*)new_buddy;
        buddy_node->next = free_list[real_order - min_order];
        free_list[real_order - min_order] = buddy_node;
    }
    //kprintf("Malloc -> Real Order : %d, target order : %d\n", (int)real_order, (int)target_order);

    block_header *header = (block_header*)to_alloc;
    header->order=target_order;

    void *usr_ptr=(void*)((char*)to_alloc + sizeof(block_header));
    return usr_ptr;
}

/* Funciones Auxiliares para el free */
static void *getBuddyAddr(void *buddy_addr, int order) {
    size_t block_size = (1 << order);
    return (void*)((size_t)buddy_addr ^ block_size);
}

static block_header *get_block_header(void *block_addr) {
    block_header *header = (block_header *)((char *)block_addr - sizeof(block_header));
    return header;
}

static size_t kmin(size_t a, size_t b) {
    return (a < b) ? a : b;
}

void buddy_free(void *block) {
    if (block == NULL) {
        return;
    }
        
    block_header *header = get_block_header(block);
    int order = header->order;
    void *block_addr = (void*)header;

    if (order > max_order || order < min_order) {
        kprintf("Free Err -> Illegal order %d (min %d, max %d)!\n", 
               order, min_order, max_order);
        return;
    }

    // Validación extensiva de dirección
    size_t block_size = (1 << order);
    if ((char*)block_addr < heap_base || 
        (char*)block_addr >= heap_base + heap_total_size ||
        ((size_t)block_addr - (size_t)heap_base) % block_size != 0) {
        kprintf("Free Err -> Illegal Address %d (heap: %d - %d) size: %d!\n",
               (int)block_addr, (int)heap_base, (int)(heap_base + heap_total_size), block_size);
        return;
    }    

    while (order < max_order) {
        void *buddy_addr = getBuddyAddr(block_addr, order);
        int clist_idx = order - min_order;
        free_node **current = &free_list[clist_idx];
        bool found_and_free=false;
        while (*current != NULL) {
            if ((void*)*current == buddy_addr) {
                *current = (*current)->next;
                found_and_free=true;
                break;
            }
            current = &(*current)->next;
        }
        if (found_and_free) {
            block_addr = (void*)(kmin((size_t)block_addr, (size_t)buddy_addr));
            block_header *cheader = (block_header*)block_addr;
            cheader->order = order++;
        } else {
            break;
        }
    }
    free_node *new_free_node=(free_node*)block_addr;
    int flist_idx = order - min_order;
    new_free_node->next = free_list[flist_idx];
    free_list[flist_idx]=new_free_node;
}
