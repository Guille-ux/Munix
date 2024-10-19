#define BUFFER_SIZE 200

#include "include/io.h"
#include "include/errors.h"
#include "include/disk.h"
#include "include/timer.h"
#include "include/speaker.h"
#include "include/ports.h"
#include "include/types.h"
#include "include/stnio.h"
#include "include/ciphers.h"
#include "include/math.h"
#include "include/display.h"
#include "include/shell.h"

void kernel_entry() {
    init_mat();
    print("Viva Stalin!", 12, 0);
    int init_pos = 12;
    const char buffer[BUFFER_SIZE];
    shell();
}
