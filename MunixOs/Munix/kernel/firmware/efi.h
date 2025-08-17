#ifndef EFI_H
#define EFI_H

#include <stddef.h>
#include <stdint.h>
#include "efidef.h"

typedef struct {
	uint64_t Signature;
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t crc32;
	uint32_t reserved;
} EFI_TABLE_HEADER;

#define EFI_TABLE_SIGNATURE 0x5453595320494249

#define EFI_REVISION_2_1 ((2 << 16) | 10)
#define EFI_REVISION_2_3 ((2 << 16) | 30)
#define EFI_REVISION_2_3_1 ((2 << 16) | 31)
#define EFI_REVISION_2_10 ((2 << 16) | 100)
#define EFI_REVISION_2_10_1 ((2 << 16) | 101)
#define EFI_REVISION_2_9 ((2 << 16) | 90)
#define EFI_REVISION_2_8 ((2 << 16) | 80)
#define EFI_REVISION_2_7 ((2 << 16) | 70)
#define EFI_REVISION_2_6 ((2 << 16) | 60)
#define EFI_REVISION_2_5 ((2 << 16) | 50)
#define EFI_REVISION_2_4 ((2 << 16) | 40)
#define EFI_REVISION_2_2 ((2 << 16) | 20)
#define EFI_REVISION_2_0 ((2 << 16) | 00)
#define EFI_REVISION_1_1 ((1 << 16) | 10)
#define EFI_REVISION_1_02 ((1 << 16) | 02)

// NOTA: para la checksum es CRC CCITT32

#define EFI_CRC_CCITT32_SEED 0x04C11DB7


typedef struct {
	EFI_TABLE_HEADER Header;
	uint16_t *FirmwareVendor;
	uint32_t FirmwareRevision;

} EFI_SYSTEM_TABLE;

#endif
