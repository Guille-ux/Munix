/************************kernel_funcs**************************/

int Kernel_Main()
{
char *str = "Hello World", *ch;
unsigned short *vidmem = (unsigned short*) 0xb8000;
unsigned i;
for (ch = str, i = 0; *ch; ch++, i++)
vidmem[i] = (unsigned char) *ch | 0x0700;
return 0;
}
