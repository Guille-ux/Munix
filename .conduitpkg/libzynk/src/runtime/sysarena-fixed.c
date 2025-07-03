/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.
 Copyright (c) 2025 Guillermo Leira Temes
*/

#include "../sysarena/types.h"
#include "../sysarena/sysarena.h"

void poor_arena_init(Arena *arena) {
    if (!arena) return;
    arena->size = 0;
    arena->used = 0;
    arena->base = NULL;
    arena->in_use = false;
    arena->is_contiguous = false;
}

bool arena_init(Arena *arena, size_t size, ptr_t base) {
    if (!arena || !base) return false;
    arena->size = size;
    arena->base = base;
    arena->used = 0;
    arena->in_use = true;
    arena->is_contiguous = true;
    return true;
}

bool sysarena_init(ArenaManager *manager, uint8_t *memory, Arena *arenas, size_t total_size, size_t num_arenas) {
    if (!manager || !memory || !arenas || num_arenas < 1) return false;
    manager->arenas = arenas;
    manager->max_arenas = num_arenas;
    manager->initial_memory = memory;
    manager->initial_size = total_size;
    manager->current_arena_idx = 0;

    poor_arena_init(&manager->arenas[0]);
    arena_init(&manager->arenas[0], total_size, memory);
    for (size_t i = 1; i < num_arenas; i++) {
        poor_arena_init(&manager->arenas[i]);
    }
    return true;
}

void* sysarena_alloc(ArenaManager *manager, size_t size) {
    if (!manager || size == 0) return NULL;

    size_t source_arena_idx = 0;
    while (source_arena_idx < manager->max_arenas && (!manager->arenas[source_arena_idx].in_use || (manager->arenas[source_arena_idx].size - manager->arenas[source_arena_idx].used) < size)) {
        source_arena_idx++;
    }

    if (source_arena_idx >= manager->max_arenas) {
        return NULL;
    }

    Arena original_source_arena_data = manager->arenas[source_arena_idx];

    void* allocated_base_address = (uint8_t*)original_source_arena_data.base + original_source_arena_data.used;

    if ((original_source_arena_data.size - original_source_arena_data.used) > size) {
        if (source_arena_idx + 1 >= manager->max_arenas) {
            return NULL;
        }

        for (size_t i = manager->max_arenas - 1; i > source_arena_idx; --i) {
            manager->arenas[i] = manager->arenas[i - 1];
        }

        Arena *allocated_arena_ptr = &manager->arenas[source_arena_idx];
        allocated_arena_ptr->base = allocated_base_address;
        allocated_arena_ptr->size = size;
        allocated_arena_ptr->used = size;
        allocated_arena_ptr->in_use = true;
        allocated_arena_ptr->is_contiguous = true;

        Arena *remainder_arena_ptr = &manager->arenas[source_arena_idx + 1];
        remainder_arena_ptr->base = (uint8_t*)allocated_base_address + size;
        remainder_arena_ptr->size = original_source_arena_data.size - original_source_arena_data.used - size;
        remainder_arena_ptr->used = 0;
        remainder_arena_ptr->in_use = (remainder_arena_ptr->size > 0);
        remainder_arena_ptr->is_contiguous = true;

        return allocated_arena_ptr->base;

    } else {
        manager->arenas[source_arena_idx].base = allocated_base_address;
        manager->arenas[source_arena_idx].size = size;
        manager->arenas[source_arena_idx].used = size;
        manager->arenas[source_arena_idx].in_use = true;
        return manager->arenas[source_arena_idx].base;
    }
}

bool sysarena_free(ArenaManager *manager, void *ptr) {
    if (!manager || !ptr) return false;
    for (size_t i = 0; i < manager->max_arenas; i++) {
        Arena *arena = &manager->arenas[i];
        if (arena->in_use && arena->base &&
            (uint8_t*)ptr >= (uint8_t*)arena->base && (uint8_t*)ptr < (uint8_t*)arena->base + arena->size) {
            poor_arena_init(arena);
            sysarena_defragment(manager);
            return true;
        }
    }
    return false;
}

void sysarena_defragment(ArenaManager *manager) {
    if (!manager) return;
    for (size_t i = 0; i < manager->max_arenas - 1; i++) {
        Arena *a = &manager->arenas[i];
        Arena *b = &manager->arenas[i + 1];
        if (!a->in_use && !b->in_use && a->size > 0 && b->size > 0 &&
            (uint8_t*)a->base + a->size == (uint8_t*)b->base) {
            a->size += b->size;
            poor_arena_init(b);
            for (size_t j = i + 1; j < manager->max_arenas - 1; j++) {
                manager->arenas[j] = manager->arenas[j + 1];
            }
            poor_arena_init(&manager->arenas[manager->max_arenas - 1]);
            i--;
        }
    }
}

bool arena_can_merge(const Arena *a, const Arena *b) {
    return (!a->in_use && !b->in_use);
}
bool arena_is_void(const Arena *a) {
    return a->used == 0;
}
bool arena_merge(Arena *dest, Arena *src) {
    if (!arena_can_merge(dest, src)) return false;
    dest->size += src->size;
    poor_arena_init(src);
    return true;
}
void copy_arena(Arena *dest, const Arena *src) {
    if (!dest || !src) return;
    dest->size = src->size;
    dest->used = src->used;
    dest->base = src->base;
    dest->in_use = src->in_use;
    dest->is_contiguous = src->is_contiguous;
}
void sysarena_displacement(ArenaManager *manager, size_t where) {
}
bool sysarena_split(ArenaManager *manager, size_t index, size_t size) {
    return false;
}
bool sysarena_is_fully_merged(ArenaManager *manager) {
    if (!manager) return false;
    size_t active_arenas = 0;
    for (size_t i = 0; i < manager->max_arenas; i++) {
        if (manager->arenas[i].in_use) active_arenas++;
    }
    return active_arenas == 1 &&
           manager->arenas[0].size == manager->initial_size &&
           manager->arenas[0].base == manager->initial_memory;
}
