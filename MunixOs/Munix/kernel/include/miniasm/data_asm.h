#ifndef MINIASM_DATA_ASM_H
#define MINIASM_DATA_ASM_H

#include "directives.h"
#include "stream.h"
#include <stdint.h>



void assemble_dd(size_t value, ByteStream *stream); // 32 bits
void assemble_dw(size_t value, ByteStream *stream); // 16 bits
void assemble_db(size_t value, ByteStream *stream); // 8 bits

void handle_data_directive(ByteStream *stream, directive dir, const char *data, size_t *pos);

#endif
