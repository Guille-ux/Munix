// i cant put copyright to this because i read OSDEV to write this directly F

//version of ACPI 1.0
struct RSDP_t { //thanks OSDEV, thanks
  char Signature[8];
  uint8_t checksum;
  char OEMID[6];
  uint8_t Revision;
  uint32_t RsdtAddress;
} __attribute__ ((packed));

// version 2.0
struct XSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;      // deprecated since version 2.0

 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed));

//Thanks osdev
