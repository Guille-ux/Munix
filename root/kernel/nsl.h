#include "keyboard.h"
#include "print.h"
#define beff
#define boff
extern boff;
extern beff;
int shell()
{
  while (boff != "exit"){
    beff = Nkhandler();
    if (beff != "\n")
    {
      boff += beff;
     }
   else
    {
      if boff == "asm"
      {
       asmshell();
      }
      else
      {
      char *str = "[!] Error: command not found [!]";
      print(str);
      }
    }
  }
}
int asmshell()
{
  while (boff != "exit")
  {
    if (boff != "exit")
    {
      asm volatile(boff);
    }
    else
    {
    shell();
    }
  }
}
