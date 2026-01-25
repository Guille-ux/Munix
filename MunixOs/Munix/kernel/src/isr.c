#include "../include/isr.h"
#include "../include/libcs2.h"
#include "../include/timer.h"
#include "../include/pic.h"
#include "../include/shell.h"

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun (Reserved)",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved", // 0x0F
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", // 0x16
    "Reserved", // 0x17
    "Reserved", // 0x18
    "Reserved", // 0x19
    "Reserved", // 0x1A
    "Reserved", // 0x1B
    "Reserved", // 0x1C
    "Reserved", // 0x1D
    "Reserved", // 0x1E
    "Reserved"  // 0x1F
}; // 32 exceptions



void isr_handler(registers_t *regs) {
	//kprintf("Interrupt, Number -> %d \n", (int)regs->int_no);
	if (regs->int_no < 32) {
        kprintf("\n-> FATAL ERROR: %s \n\t -> Int Number : %d \n", (const char*)exception_messages[regs->int_no], regs->int_no);
		//__asm__ volatile("cli");
        //__asm__ volatile("halt");
        while (1);
		stdlog_interface.append(exception_messages[regs->int_no]);
		// 2 opciones, halt o añadirlo al log e ignorarlo, yo lo ignoro
	}  else if (regs->int_no > 31 && regs->int_no < 48) { // lo hare con otros handlers especiales
		if (regs->int_no==32) {
            timer_handler_2();
            pic_eoi(0);
            return;
        } else if (regs->int_no==33) {
            kernel_keyboard_handler_2(); // sera sustituido por el nuevo
            pic_eoi(1);
            return;
        }
	}
}



/*
void kernel_keyboard_handler(uint32_t *esp) {
    __asm__ volatile("cli");
    keyboard_handler();
    if (char_out && final_character != 0) {
        shell_event=true;
        //push_to_buffer(final_character);
        shell_event=true;
        shell_buffer[shell_index++]=final_character;
        kprintf(" %s ", (const char*)shell_buffer);
        //kprintf("%s ", (const char*)final_character);
        
    } else if (scancode==special_layout.backspace) {
        backspace=true;
    } else if (scancode==special_layout.enter) {
        send=true;
    }
    pic_eoi(1);
    __asm__ volatile("sti");
}
*/
void kernel_keyboard_handler_2() {
    keyboard_handler();
    if (char_out && final_character != 0) {
        shell_event=true;
        //push_to_buffer(final_character);
        /*shell_event=true;
        shell_buffer[shell_index++]=final_character;
        kprintf(" %s ", (const char*)shell_buffer);
        //kprintf("%s ", (const char*)final_character);
        */
    } else if (scancode==special_layout.backspace) {
        backspace=true;
    } else if (scancode==special_layout.enter) {
        send=true;
    }
}
