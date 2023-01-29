#include "keyboard.h"
#include "print.h"
#include "string.h"
#define beff
#define boff

extern boff;
extern beff;

int shell()
{
  while (1) {
    beff = Nkhandler();
    if (beff != '\n') {
      boff += beff;
    } else {
      if (strcmp(boff, "exit") == 0) {
        break;
      } else if (strcmp(boff, "asm") == 0) {
        asmshell();
      } else {
        char *str = "[!] Error: command not found [!]";
        print(str);
      }
      boff = "";
    }
  }
}

int asmshell()
{
  while (1) {
    beff = Nkhandler();
    if (beff != '\n') {
      boff += beff;
    } else {
      if (strcmp(boff, "exit") == 0) {
        break;
      } else {
        asm volatile(boff);
      }
      boff = "";
    }
  }
}
