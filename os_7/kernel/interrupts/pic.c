#include <stdint.h>
#include "interrupts.h"
#include "utils.h"

void irq_null_handler0();
void irq_null_handler1();

void init_pic()
{
    uint8_t i;
    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, 0xfb);
    io_wait();
    outb(PIC2_DATA, 0xff);
    io_wait();

    struct I_DESC irq_null = make_int_desc(8, (uint32_t)&irq_null_handler0);
    for(i = 0x20; i <= 0x27; ++i)
        IDT[i] = irq_null;
    irq_null = make_int_desc(8, (uint32_t)&irq_null_handler1);
    for(i = 0x28; i <= 0x30; ++i)
        IDT[i] = irq_null;
}

void pic_eoi(uint8_t irq)
{
    if(irq >= 8)
        outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}

void enable_irq(uint8_t irq)
{
    uint8_t m;
    if(irq >= 8) {
        m = inb(PIC2_DATA);
        m &= ~(0x1 << (irq - 8));
        outb(PIC2_DATA, m);
    } else {
        m = inb(PIC1_DATA);
        m &= ~(0x1 << irq);
        outb(PIC1_DATA, m);
    }
}
