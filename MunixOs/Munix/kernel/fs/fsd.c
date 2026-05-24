#include "fsd.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include <stdint.h>

extern char _kernel_fds_start;
extern char _kernel_fds_end;

fd_t *kernel_fds = &_kernel_fds_start;
uint32_t kernel_fds_size = 0;

void initFd() {
	kernel_fds_size = ((uint32_t)&_kernel_fds_end - (uint32_t)&kernel_fds_start) / sizeof(fd_t);
	memset(&_kernel_fds_start, 0, kernel_fds_size);
}
