/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * Copyright (c) 2025 Guillermo Leira Temes
 */

#include "../include/ksysarena.h"
#include "../include/string.h"

void kpoor_arena_init(Arena *arena) {
    if (!arena) return;
    arena->size = 0;
    arena->used = 0;
    arena->base = NULL;
    arena->in_use = false;
    arena->is_contiguous = false; 
}

bool karena_init(Arena *arena, size_t size, ptr_t base) {
    if (!arena || !base) return false;
    arena->size = size;
    arena->base = base;
    arena->used = 0;          
    arena->in_use = false;    
                              
    arena->is_contiguous = true; 
    return true;
}


bool ksysarena_init(ArenaManager *manager, uint8_t *memory, Arena *arenas, size_t total_size, size_t num_arenas) {
    if (!manager || !memory || !arenas || num_arenas < 1) return false;

    manager->arenas = arenas;
    manager->max_arenas = num_arenas;
    manager->initial_memory = memory;
    manager->initial_size = total_size;
    manager->current_arena_idx = 0; 
    
    karena_init(&manager->arenas[0], total_size, memory);

    
    for (size_t i = 1; i < num_arenas; i++) {
        kpoor_arena_init(&manager->arenas[i]);
    }
    return true;
}



void* ksysarena_alloc(ArenaManager *manager, size_t size) {
    if (!manager || size == 0) return NULL;


    size_t source_arena_idx = 0;
    while (source_arena_idx < manager->max_arenas &&
           (manager->arenas[source_arena_idx].in_use || 
            (manager->arenas[source_arena_idx].size < size))) { 
        source_arena_idx++;
    }

    if (source_arena_idx >= manager->max_arenas) {
        return NULL;
    }

    Arena *target_free_arena = &manager->arenas[source_arena_idx];

    void* allocated_address = target_free_arena->base;

    
    if ((target_free_arena->size - size) > 0) { 
        size_t new_free_arena_idx = manager->max_arenas;
        for (size_t i = 0; i < manager->max_arenas; i++) {
            if (!manager->arenas[i].in_use && manager->arenas[i].base == NULL) { 
                new_free_arena_idx = i;
                break;
            }
        }
        
        if (new_free_arena_idx >= manager->max_arenas) {
            if (target_free_arena->size == size) {
                target_free_arena->in_use = true;
                target_free_arena->used = size;
                return allocated_address;
            }
            return NULL;
        }

        target_free_arena->size = size;
        target_free_arena->used = size;
        target_free_arena->in_use = true;

        Arena *remainder_arena = &manager->arenas[new_free_arena_idx];
        remainder_arena->base = (uint8_t*)allocated_address + size;
        remainder_arena->size = (target_free_arena->size + target_free_arena->used) - size; // Correct calculation for remainder
        remainder_arena->used = 0;          
        remainder_arena->in_use = false;    
        remainder_arena->is_contiguous = true; 

	ksysarena_defragment(manager);

        return allocated_address;

    } else {
        target_free_arena->in_use = true;
        target_free_arena->used = size; 
        return allocated_address;
    }
}


bool ksysarena_free(ArenaManager *manager, void *ptr) {
    if (!manager || !ptr) return false;

    for (size_t i = 0; i < manager->max_arenas; i++) {
        Arena *arena = &manager->arenas[i];
        
        if (arena->in_use && arena->base == ptr) { 
            arena->in_use = false;
            arena->used = 0;
            ksysarena_defragment(manager);
            return true;
        }
    }
    return false; 
}


void ksysarena_defragment(ArenaManager *manager) {
    if (!manager) return;
    for (size_t i = 0; i < manager->max_arenas; i++) {
        if (!manager->arenas[i].in_use && manager->arenas[i].size > 0) {
            for (size_t j = i + 1; j < manager->max_arenas; j++) {
                if (!manager->arenas[j].in_use && manager->arenas[j].size > 0) {
                    if ((uint8_t*)manager->arenas[i].base + manager->arenas[i].size == (uint8_t*)manager->arenas[j].base) {
                        manager->arenas[i].size += manager->arenas[j].size; // Merge
                        kpoor_arena_init(&manager->arenas[j]);
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < manager->max_arenas; i++) {
        if (manager->arenas[i].base == NULL && manager->arenas[i].size == 0) {
            for (size_t j = i + 1; j < manager->max_arenas; j++) {
                if (manager->arenas[j].base != NULL || manager->arenas[j].size != 0) { 
                    manager->arenas[i] = manager->arenas[j]; 
                    kpoor_arena_init(&manager->arenas[j]); 
                    break;
                }
            }
        }
    }
}


bool karena_can_merge(const Arena *a, const Arena *b) {
    return (!a->in_use && !b->in_use);
}

bool karena_is_void(const Arena *a) {
    return a->used == 0 && a->size == 0 && a->base == NULL;
}

bool karena_merge(Arena *dest, Arena *src) {
    if (!karena_can_merge(dest, src)) return false;
    dest->size += src->size;
    kpoor_arena_init(src);
    return true;
}

void kcopy_arena(Arena *dest, const Arena *src) {
    if (!dest || !src) return;
    dest->size = src->size;
    dest->used = src->used;
    dest->base = src->base;
    dest->in_use = src->in_use;
    dest->is_contiguous = src->is_contiguous;
}

bool ksysarena_is_fully_merged(ArenaManager *manager) {
    if (!manager) return false;
    size_t active_arenas = 0;
    for (size_t i = 0; i < manager->max_arenas; i++) {
        if (manager->arenas[i].in_use || manager->arenas[i].size > 0) active_arenas++;
    }
    return active_arenas == 1 &&
           manager->arenas[0].size == manager->initial_size &&
           manager->arenas[0].base == manager->initial_memory &&
           !manager->arenas[0].in_use;
}