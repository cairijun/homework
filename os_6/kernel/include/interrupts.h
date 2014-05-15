#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H
#include <stdint.h>
#include "kernel.h"
#include "keyboard.h"

struct I_DESC
{
    uint16_t offset_low;
    uint16_t seg_sec;
    uint16_t                : 5;
    uint16_t _FIXED         : 8;
    uint16_t priv_level     : 2;
    uint16_t present        : 1;
    uint16_t offset_high;
}__attribute__((packed));

extern struct I_DESC *IDT;

void init_idt();
struct I_DESC make_int_desc(uint16_t selector, uint32_t offset);

#define PIC1             0x20
#define PIC2             0xA0
#define PIC1_COMMAND     PIC1
#define PIC1_DATA        (PIC1+1)
#define PIC2_COMMAND     PIC2
#define PIC2_DATA        (PIC2+1)

#define ICW1_ICW4        0x01
#define ICW1_SINGLE      0x02
#define ICW1_INTERVAL4   0x04
#define ICW1_LEVEL       0x08
#define ICW1_INIT        0x10

#define ICW4_8086        0x01
#define ICW4_AUTO        0x02
#define ICW4_BUF_SLAVE   0x08
#define ICW4_BUF_MASTER  0x0C
#define ICW4_SFNM        0x10

void init_pic();
void pic_eoi(uint8_t irq);
void enable_irq(uint8_t irq);

static inline void cli()
{
    __asm__ volatile ("cli;");
}

static inline void sti()
{
    __asm__ volatile ("sti;");
}

#endif
