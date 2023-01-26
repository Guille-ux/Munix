uint8_t IpB(uint16_t _port)
{
unsigned char rv;
asm volatile("inb %1, %0" : "=a"(rv) : "dN"(_port));
return rv;
}
void PoB(uint16_t port, uint8_t value)
{
asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}
