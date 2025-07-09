#ifndef PIC_H
#define PIC_H

#include "libcs2.h"
#include <stdint.h>

#define PIC1_COMMAND 0x20 // Master
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xA0 // Slave
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x11


// 

#define ICW1_IC4	0x01		
#define ICW1_SINGLE	0x02		
#define ICW1_INTERVAL4	0x04	
#define ICW1_LEVEL	0x08
//#define ICW1_INIT	0x10		

#define ICW4_8086	0x01		
#define ICW4_AUTO	0x02		
#define ICW4_BUF_SLAVE	0x08		
#define ICW4_BUF_MASTER	0x0C		
#define ICW4_SFNM	0x10		

// EOI Command

#define PIC_EOI 0x20

static inline void outb_delay() {
	outb(0x80, 0);
	inb(0x80);
}

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_disable(void);
void pic_enable(void);
void pic_eoi(uint8_t irq);
void pic_mask_irq(uint8_t irq_line);
void pic_unmask_irq(uint8_t irq_line);

#endif
