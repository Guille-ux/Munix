#include <stdio.h>
#include <stdint.h>
#include "fdp.h"

fdp_t fdp_add(fdp_t a, fdp_t b) {
	fdp_t result;
	a = fdp_simplify(a);
	b = fdp_simplify(b);
	char exp_a = fdp_exp_char(a);
	char exp_b = fdp_exp_char(b);
	char rexp;
	if (exp_a < exp_b) {
		uint32_t z = exp_b - exp_a;
		while (z-- > 0) {
			a = fdp_order_sub(a);
		}
		rexp = exp_b;
	} else if (exp_b < exp_a) {
		uint32_t z = exp_a - exp_b;
		while (z-- > 0) {
			b = fdp_order_sub(b);
		}
		rexp = exp_a;
	}
	result = fdp_char_exp(result, rexp);
	result.Mantissa = a.Mantissa + b.Mantissa;
	return fdp_simplify(result);
}
fdp_t fdp_sub(fdp_t a, fdp_t b) {
	fdp_t result;
	a = fdp_simplify(a);
	b = fdp_simplify(b);
	char exp_a = fdp_exp_char(a);
	char exp_b = fdp_exp_char(b);
	char rexp;
	if (exp_a < exp_b) {
		uint32_t z = exp_b - exp_a;
		while (z-- > 0) {
			a = fdp_order_sub(a);
		}
		rexp = exp_b;
	} else if (exp_b < exp_a) {
		uint32_t z = exp_a - exp_b;
		while (z-- > 0) {
			b = fdp_order_sub(b);
		}
		rexp = exp_a;
	}
	result = fdp_char_exp(result, rexp);
	result.Mantissa = a.Mantissa - b.Mantissa;
	return fdp_simplify(result);
}
fdp_t fdp_mul(fdp_t a, fdp_t b) {
	fdp_t result;
	a = fdp_simplify(a);
	b = fdp_simplify(b);
	result.Mantissa = a.Mantissa * b.Mantissa; 
	result = fdp_char_exp(result, fdp_exp_char(a)+fdp_exp_char(b));
	return fdp_simplify(result);
}
fdp_t fdp_div(fdp_t a, fdp_t b) {
	fdp_t result;
	a = fdp_extend(a, EXTEND_VAL);
	b = fdp_extend(b, EXTEND_VAL);
	result.Mantissa = a.Mantissa / b.Mantissa; 
	result = fdp_char_exp(result, fdp_exp_char(a)-fdp_exp_char(b));
	return fdp_simplify(result);
}

fdp_t fdp_simplify(fdp_t num) {
	char exp = fdp_exp_char(num);
	while (num.Mantissa % 10 == 0) {
		num.Mantissa /= 10;
		exp += 1;
	}
	num = fdp_char_exp(num, exp);
	return num;
}

uint32_t powd(uint32_t n) {
	uint32_t tmp = 1;
	while (n-- > 0) {
		tmp *= 10;
	}
	return tmp;
}

char fdp_exp_char(fdp_t num) {
	return (num.Exp & 0xF) | (num.Enega << 7);
}

fdp_t fdp_char_exp(fdp_t num, char exp) {
	num.Exp = (uint32_t)(exp & 0xF);
	num.Enega = (exp & (1 << 7) ? 1 : 0);
	return num;
}

fdp_t fdp_order_sub(fdp_t num) {
	char exp = fdp_exp_char(num);
	num = fdp_char_exp(num, exp-1);
	num.Mantissa *= 10;
	return num;
}

fdp_t fdp_order_inc(fdp_t num) {
	char exp = fdp_exp_char(num);
	num = fdp_char_exp(num, exp+1);
	num.Mantissa /= 10;
	return num;
}

fdp_t fdp_extend(fdp_t num, uint32_t times) {
	while (times-- > 0) {
		num = fdp_order_sub(num);
	}
	return num;
}
