#ifndef DISK_COMMON_H
#define DISK_COMMON_H

#include "../include/libcs2.h"

typedef struct {
	uint32_t lo;
	uint16_t hi;
} lba_t;

static inline lba_t uint64_2_lba(uint64_t address) {
	lba_t lba;
	lba.lo = (uint32_t)(address & 0xFFFFFFFF);
	lba.hi = (uint16_t)((address << 31) & 0xFFFF);
	return lba;
}

static inline uint64_t lba2uint64(lba_t address) {
	uint64_t addr=0;
	addr |= (uint64_t)address.lo;
	addr |= (uint64_t)address.hi >> 31;
	return addr;
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
