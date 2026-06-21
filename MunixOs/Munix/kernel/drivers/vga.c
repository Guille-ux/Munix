#include "vga.h"

int vga_read(file_t  *file, void *buffer, size_t size) {
	if (buffer == NULL) return -1;
	int read = size < ? size : _VGA_SIZE;
	memcpy(buffer, (void*)_VGA_BASE_DIRECTION, read);
	return read;
}

int vga_write(file_t *file, void *buffer, size_t size) {
	if (buffer == NULL) return -1;
	int written = size < ? size : _VGA_SIZE;
	memcpy((void*)_VGA_BASE_DIRECTION, buffer, written);
	return written;
}

int vga_extend(file_t *file) {
	return -1;
}

int init_vga_fd(int fd) {
	kernel_fds[fd].ref_count=1;
	kernel_fds[fd].file.read = vga_read;
	kernel_fds[fd].file.write = vga_write;
	kernel_fds[fd].file.extend = vga_extend;
	return 0;
}
