#ifndef MULTIBOOT_DEF
#define MULTIBOOT_DEF

typedef enum {
	MULTIBOOT_MEM_AVAILABLE=1,
	MULTIBOOT_MEM_RESERVED=2,
	MULTIBOOT_MEM_ACPI_RECLAIMABLE=3,
	MULTIBOOT_MEM_NVS=4,
	MULTIBOOT_MEM_BADRAM=5,
} multiboot_mem_type; 

struct multiboot_aout_symbol_table {
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
};

struct multiboot_elf_section_header_table {
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};

typedef struct multiboot_mem_map {
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
} multiboot_mem_map_t;

typedef struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_dev;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	union {
		struct multiboot_aout_symbol_table aout_syms;
		struct multiboot_elf_section_header_table elf_sections;
	} syms;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;

#endif
