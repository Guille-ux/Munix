#include "keyboard.h"

extern char _kernel_fds_start;

extern fd_t *kernel_fds;

int kb_read(file_t  *file, void *buffer, size_t size) {
	if (buffer==NULL) return -1;
	int read;
	uint16_t *buff = (uint16_t*)buffer;
	for (read=0;read<size;read++) {
		buff[read] = kgetchar();
	}
	return read;
}

int kb_write(file_t *file, void *buffer, size_t size) {
	return -1; // como no tengo ni idea de como hacer esto, ps simplemente lo prohibimos
}


int kb_extend(file_t *file) {
	return -1;
}

int init_keyboard_fd(int fd) {
	kernel_fds[fd].ref_count=1;
	kernel_fds[fd].file.read = kb_read;
	kernel_fds[fd].file.write = kb_write;
	kernel_fds[fd].file.extend = kb_extend;
	return 0;
}
