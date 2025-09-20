#ifndef FDP_H
#define FDP_H

#include <stdint.h>

#define EXTEND_VAL 5

typedef struct {
	uint32_t Negative : 1;
	uint32_t Enega : 1;
	uint32_t Exp : 4;
	uint32_t Mantissa : 26;
} fdp_t; // floating decimal point

fdp_t fdp_add(fdp_t a, fdp_t b);
fdp_t fdp_sub(fdp_t a, fdp_t b);
fdp_t fdp_mul(fdp_t a, fdp_t b);
fdp_t fdp_div(fdp_t a, fdp_t b);
fdp_t fdp_simplify(fdp_t num);
char fdp_exp_char(fdp_t num);
fdp_t fdp_char_exp(fdp_t num, char exp);
fdp_t fdp_order_sub(fdp_t num);
fdp_t fdp_order_inc(fdp_t num);
fdp_t fdp_extend(fdp_t num, uint32_t times);

uint32_t powd(uint32_t n);

#endif
