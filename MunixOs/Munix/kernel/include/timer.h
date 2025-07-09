#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define PIT_FREQ 1193182 // Aprox

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43


extern volatile uint32_t tick_counter;

void timer_init(int hz);
void timer_handler(uint32_t *esp);
void register_timer_handler(void);

#endif
