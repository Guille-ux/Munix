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

#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>  
#include <stdint.h>  
#include <stdbool.h> 

//tipos de punteros
typedef void* ptr_t;

//punteros uint
typedef unsigned char* uint8_ptr_t;
typedef unsigned short* uint16_ptr_t;
typedef unsigned int* uint32_ptr_t;
typedef unsigned long long* uint64_ptr_t;

//punteros int
typedef signed char* int8_ptr_t;
typedef signed short* int16_ptr_t;
typedef signed int* int32_ptr_t;
typedef signed long long* int64_ptr_t;

#endif
