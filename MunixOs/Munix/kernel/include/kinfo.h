#ifndef KERNEL_INFO_H
#define KERNEL_INFO_H

#include "../multiboot/multiboot_def.h"

extern char _kernel_data_start;
extern char _kernel_data_end;

// ire añadiendo más cosas segun lo necesite
typedef struct {
	multiboot_info_t *mbi;
} kernel_data_t;

#endif
