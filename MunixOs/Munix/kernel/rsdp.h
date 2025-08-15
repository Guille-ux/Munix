#ifndef RSDP_H
#define RSDP_H

#include <stdint.h>

#define RSDP_SIGNATURE "RSD PTR "

typedef struct {
	char Signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t address;
} __attribute__((packed)) rsdp_t;

typedef struct {
	char Signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t rsdtAddress; 
	uint32_t len;
	uint64_t xsdtAddress;
	uint8_t extendedChecksum;
	uint8_t reserved[3];
} __attribute__((packed)) xsdt_t;



#endif
