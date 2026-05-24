#include "usr.h"

extern char _kernel_identities_start;
extern char _kernel_identities_end;

identity_t *kernel_identities = &_kernel_identities_start;
uint32_t kernel_identities_size = 0;

void initIdentities() {
	kernel_identities_size = ((uint32_t)&_kernel_identities_end - (uint32_t)&kernel_identities_start) / sizeof(identity_t);
	memset(&_kernel_identities_start, 0, kernel_identities_size);
}
