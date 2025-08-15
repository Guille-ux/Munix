#ifndef EBDA_H
#define EBDA_H

#define EBDA_PTR 0x40E

static inline uint16_t *get_ebda() {
	return (uint16_t*)(EBDA_PTR*16);
}

#endif
