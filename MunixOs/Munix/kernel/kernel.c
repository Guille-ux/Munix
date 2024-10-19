#define BUFFER_SIZE 200
#define SIZE ((uint32_t)27796)

#include "include/includer.h"

void kernel_entry() {
    init_mat();
    print("Viva Stalin!", 12, 0);
    int init_pos = 12;
    const char buffer[BUFFER_SIZE];
    shell();
}
