#include "keyboard.h"
#include "print.h"
#include "string.h"

char beff[100];
char boff[100];

int shell()
{
while (strcmp(boff, "exit") != 0) {
strncat(beff, Nkhandler(), 1);
if (strcmp(beff, "\n") != 0) {
strcat(boff, beff);
} else {
if (strcmp(boff, "asm") == 0) {
asmshell();
} else {
char *str = "[!] Error: command not found [!]";
print(str);
}
}
}
return 0;
}

int asmshell()
{
while (strcmp(boff, "exit") != 0) {
if (strcmp(boff, "exit") != 0) {
asm volatile(boff);
} else {
shell();
}
}
return 0;
}
