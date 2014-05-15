#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "interrupts.h"

struct I_DESC *IDT;

void init_idt()
{
    int i;
    struct I_DESC null_handler = make_int_desc(0x8, (uint32_t)&int_null_handler);
    for(i = 0; i < 0xff; ++i) {
        IDT[i] = null_handler;
    }

    __asm__(
        ".intel_syntax noprefix;"
        "lidt [idtr];"
        ".att_syntax;"
    );
}

struct I_DESC make_int_desc(uint16_t selector, uint32_t offset)
{
    struct I_DESC res;
    res.offset_low = offset & 0xffff;
    res.seg_sec = selector;
    res._FIXED = 0x70;
    res.priv_level = 0;
    res.present = 1;
    res.offset_high = offset >> 16;
    return res;
}

