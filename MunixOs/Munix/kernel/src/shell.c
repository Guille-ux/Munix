#include "../include/libcs2.h"
#include "../include/shell.h"
#include <stdint.h>
#include <stdbool.h>

bool send=false;
bool backspace=false;
bool shell_event=false;
char *shell_prompt="~ MunixOs ~> ";
char shell_buffer[SHELL_BUFFER_SIZE];
uint32_t shell_index=0;

void push_to_buffer(char c) {
    if (!(shell_index<SHELL_BUFFER_SIZE)) return;
    shell_buffer[shell_index++]=c;
    shell_buffer[shell_index]='\0';
}

void pop_from_buffer() {
    if (shell_index==0) return;
    shell_buffer[--shell_index]='\0';
}

void shell_update() {
    kprintf("\r");
    for (int i=1;i<stdout_interface.get_max_x()/8;i++) {
        kprintf("\t");
    }
    kprintf("\r%s %s", (const char*)shell_prompt, (const char*)shell_buffer);
}