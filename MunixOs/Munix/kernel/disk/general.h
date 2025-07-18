#ifndef DISK_COMMON_H
#define DISK_COMMON_H

#include "../include/libcs2.h"

typedef struct {
	uint32_t lo;
	uint16_t hi;
} __attribute__((packed)) lba_t;

static inline lba_t uint64_2_lba(uint64_t address) {
	lba_t lba;
	lba.lo = (uint32_t)(address & 0xFFFFFFFF);
	lba.hi = (uint16_t)((address >> 32) & 0xFFFF);
	return lba;
}

typedef struct {
	uint8_t cylinder;
	uint8_t head;
	uint8_t sector;
} __attribute__((packed)) chs_t;

static void printCHS(chs_t address) {
	kprintf("\t -> C : 0x%x \n", address.cylinder);
	kprintf("\t -> H : 0x%x \n", address.head);
	kprintf("\t -> S : 0x%x \n", address.sector);
}

#endif
