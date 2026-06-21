#ifndef MUNIX_DRIVERS_VGA_H
#define MUNIX_DRIVERS_VGA_H

#include "../fs/fsd.h"
#include <stdint.h>
#include <stddef.h>

#define _VGA_BASE_DIRECTION 0xB8000
#define _VGA_SIZE 80*25*2

int vga_read(file_t  *file, void *buffer, size_t size);
int vga_write(file_t *file, void *buffer, size_t size);
int vga_extend(file_t *file);

int init_vga_fd(int fd);

#endif
