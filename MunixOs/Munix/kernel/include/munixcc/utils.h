#ifndef MUNIXCC_UTILS_H
#define MUNIXCC_UTILS_H

/*
 *
 * Algunas utilidades
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include "compat.h"

bool is_identifier_ch(char c); // es valido en identificadores?
bool is_delimiter(char c); // si delimita el final de un keyword o identifier
char *int2char(char *string, size_t len, size_t num);

#endif
