#include "../include/display.h"
#include "../include/vga.h"

MultiColor stdcolor = {.as.text_mode=(VGA_COLOR_LIGHT_GREY || VGA_COLOR_BLACK << 4), .as.graphic_mode={0, 0, 0}};
