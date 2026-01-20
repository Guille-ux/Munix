#ifndef MINIASM_COMPAT_H
#define MINIASM_COMPAT_H

/*
 * Esto debe incluir las declaraciones de:
 *
 * Malloc y Free
 * memcpy, memset y strcmp
 *
 * (NOTA: No uso nada de FOPEN etc, por compatibilidad, a cambio
 *  el ensamblador devolvera los BLOBS binarios directamente)
 *
 */

#include "../libcs2.h" // ya viene con todo

#endif
