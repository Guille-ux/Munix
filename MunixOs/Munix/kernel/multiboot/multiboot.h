#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>
#include "multiboot_def.h"

#define MULTIBOOT_INFO_MEMORY             0x00000001 
#define MULTIBOOT_INFO_BOOTDEV            0x00000002 
#define MULTIBOOT_INFO_CMDLINE            0x00000004 
#define MULTIBOOT_INFO_MODS               0x00000008 
#define MULTIBOOT_INFO_AOUT_SYMS          0x00000010 
#define MULTIBOOT_INFO_ELF_SHDR           0x00000020 
#define MULTIBOOT_INFO_MEM_MAP            0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO         0x00000080 
#define MULTIBOOT_INFO_CONFIG_TABLE       0x00000100 
#define MULTIBOOT_INFO_BOOT_LOADER_NAME   0x00000200 
#define MULTIBOOT_INFO_APM_TABLE          0x00000400 
#define MULTIBOOT_INFO_VBE_INFO           0x00000800 
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO   0x00001000

#define MULTIBOOT_1_MAGIC 0x1BADB002
#define MULTIBOOT_BOOT_MAGIC 0x2BADB002

typedef struct {
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t string;
	uint32_t reserved;
} __attribute__((packed)) multiboot_module_t;

#endif
