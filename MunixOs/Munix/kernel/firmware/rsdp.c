#include "rsdp.h"
#include <stddef.h>
#include "../include/libcs2.h"

xsdt_t *find_xsdt_bios() {
	uint32_t ebda_addr = (uint32_t)get_ebda_segment();
	ebda_addr *= 16;
	for (uint32_t i=0;i<1024;i+=16) {
		xsdt_t *xsdt = (xsdt_t*)(ebda_addr+i);
		if (memcmp(xsdt->Signature, RSDP_SIGNATURE, 8)==0) {
			return xsdt;
		}
	}
	for (uint32_t i=BMM_START;i<BMM_END;i+=16) {
		xsdt_t *xsdt = (xsdt_t*)i;
		if (memcmp(xsdt->Signature, RSDP_SIGNATURE, 8)==0) {
			return xsdt;
		}
	}
	
	return NULL;
}
