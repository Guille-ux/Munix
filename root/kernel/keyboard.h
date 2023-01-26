/***************************************************************keyboard*****************************************************************************/
//keyboard.h

#include "us_keyboard.h"
#include "print.h"
#include "sense.h"
#define BUFF = 0

void Nkhandler()
{
  unsigned char *buffkey = Nk();
  if (buffkey != 255)
  {
  print(buffkey);
  BUFF = buffkey;
  return BUFF;
  }
}
int Nk()
{
  unsigned char buffcode;
  buffcode = (unsigned char)IpB(0x60)
  if (buffcode)
  return en_US[buffcode]
}
