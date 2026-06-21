#ifndef MUNIX_DRIVERS_KEYBOARD_H
#define MUNIX_DRIVERS_KEYBOARD_H

#include "../fs/fsd.h"
#include <stdint.h>
#include <stddef.h>
#include "../keyboard/keyboard.h"

int kb_read(file_t  *file, void *buffer, size_t size);
int kb_write(file_t *file, void *buffer, size_t size);
int kb_extend(file_t *file);

int init_keyboard_fd(int fd);

#endif
