#ifndef MINIASM_MINIASM_H
#define MINIASM_MINIASM_H

#include <stdbool.h>
#include "stream.h"
#include "link.h"

#define DEFAULT_ARCH 32

void rshift_placeholder(uint8_t extender, ByteStream *stream, char *text, size_t *pos);
bool assemble_text(ByteStream *outStream, const char *otext, ByteStream *linking);
uint8_t assemble_reg(char *text, size_t *pos, char *arch);
void skipUntilThese(char *text, size_t *pos, const char *list);
void skipThese(char *text, size_t *pos, const char *list);
void modrm_placeholder(uint8_t opcode, ByteStream *stream, char *text, size_t *pos);
void i_placeholder(uint8_t extender, ByteStream *stream, char *text, size_t *pos);
void shift_placeholder(uint8_t extender, uint8_t opcode, ByteStream *stream, char *text, size_t *pos);

/*
 * Mapeo de Registros para x86-32
 *
 * EAX: 000
 * ECX: 001
 * EDX: 010
 * EBX: 011
 * ESP: 100
 * EBP: 101
 * ESI: 110
 * EDI: 111
 *
 */

#endif
