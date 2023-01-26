/**********Kernel**************/
//Nexus.c

#include "colors.h"
#include "print.h"
#include "nsl.h"

int main()
{
char *str = "Hello World";
char *std = "-----------";
print(*str);
print(std);
shell();
}
