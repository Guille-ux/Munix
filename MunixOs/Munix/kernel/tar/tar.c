#include "tar.h"

size_t oct2int(char *chain, char length) {
	size_t size = 0;
	
	for (int i=0;i < length-1;i++) {
		size += chain[i] - '0';
		size *= 8;
	}

	return size;
}


void int2oct(char *chain, char length, size_t n) {
	size_t size = 0;
	
	for (int i=length-1;i > 0;i--) {
		chain[i] = n % 8 + '0';
		n /= 8;
	}
}
