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

#include <stddef.h>
#include <stdint.h>

#ifndef _SYSARENA_H
#define _SYSARENA_H


typedef void* ptr_t;

typedef struct Arena {
    size_t size;         // Tamaño total del bloque
    ptr_t base;          // Puntero al inicio del bloque
    size_t used;         // Bytes usados en el bloque
    bool in_use;         // ¿Bloque ocupado?
    bool is_contiguous;  // ¿Es contiguo para fusionar?
} Arena;

typedef struct ArenaManager {
    Arena* arenas;            // Array de arenas (bloques)
    size_t max_arenas;        // Número total de arenas
    uint8_t *initial_memory;  // Memoria inicial global
    size_t initial_size;      // Tamaño inicial global
    size_t current_arena_idx; // No es crítico para el modelo génesis, pero puede usarse
} ArenaManager;

#endif

#ifndef KSYSARENA
#define KSYSARENA

// Inicialización
bool ksysarena_init(ArenaManager *manager, uint8_t *memory, Arena *arenas, size_t total_size, size_t num_arenas);

// Reservar memoria según el sistema génesis
void* ksysarena_alloc(ArenaManager *manager, size_t size);

// Liberar un bloque entero
bool ksysarena_free(ArenaManager *manager, void *ptr);

// Fusionar arenas libres y contiguas
void ksysarena_defragment(ArenaManager *manager);

// Inicializar arena vacía
void kpoor_arena_init(Arena *arena);

// Inicializar arena con datos
bool karena_init(Arena *arena, size_t size, ptr_t base);

// Utilidades (pueden mantenerse por compatibilidad)
void kcopy_arena(Arena *dest, const Arena *src);
bool karena_can_merge(const Arena *a, const Arena *b);
bool karena_is_void(const Arena *a);
bool karena_merge(Arena *dest, Arena *src);
bool ksysarena_is_fully_merged(ArenaManager *manager);

#endif

