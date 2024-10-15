#define PIT_FREQ 1193182
#define PIT_2 0x43    // Puerto de datos del PIT 2
#define PIT_2_W 0xB6  // Comando para configurar el PIT 2
#define SPEAKER 0x61

void speaker_on() {
	char status = inb(SPEAKER);
	outb(SPEAKER, status | 0x03);
}
void speaker_off() {
	char status = inb(SPEAKER);
	outb(SPEAKER, status & 0xFC);
}
void play_tone(int freq) {
	uint16_t divisor = PIT_FREQ / freq; // freq in khz
	outb(PIT_2, PIT_2_W);
	outb(PIT_2-1, divisor & 0xFF);
	outb(PIT_2-1, (divisor >> 8) & 0xFF);
}
