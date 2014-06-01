#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "interrupts.h"
#include "scheduler.h"
#include "keyboard.h"
#include "keyboard_map.h"

#define SET_KBSTATUS(s) do{ if(pressed) KBBUF.status |= (s);\
                            else        KBBUF.status &= ~(s);} while(0)

struct KeyboardBuf_t KBBUF;

void init_shift_map()
{
    int8_t i;
    for(i = 'a'; i <= 'z'; ++i)
        SHIFTMAP[i] = i - 'a' + 'A';
    SHIFTMAP['`'] = '~';
    SHIFTMAP['1'] = '!';
    SHIFTMAP['2'] = '@';
    SHIFTMAP['3'] = '#';
    SHIFTMAP['4'] = '$';
    SHIFTMAP['5'] = '%';
    SHIFTMAP['6'] = '^';
    SHIFTMAP['7'] = '&';
    SHIFTMAP['8'] = '*';
    SHIFTMAP['9'] = '(';
    SHIFTMAP['0'] = ')';
    SHIFTMAP['-'] = '_';
    SHIFTMAP['='] = '+';
    SHIFTMAP['['] = '{';
    SHIFTMAP[']'] = '}';
    SHIFTMAP['\\'] = '|';
    SHIFTMAP[';'] = ':';
    SHIFTMAP['\''] = '"';
    SHIFTMAP[','] = '<';
    SHIFTMAP['.'] = '>';
    SHIFTMAP['/'] = '?';
}

void init_kb()
{
    init_kbbuf(&KBBUF);
    init_shift_map();
    IDT[0x21] = make_int_desc(8, (uint32_t)inthandler21);
    enable_irq(1);
}

void handle_extended_key(uint8_t scan_code, bool pressed)
{
    switch(KEYMAP[scan_code]) {
        case CTRL:
            SET_KBSTATUS(STATUS_CTRL); break;
        case ALT:
            SET_KBSTATUS(STATUS_ALT); break;
        default:
            if(pressed)
                enq_kbbuf(&KBBUF, scan_code | (KBBUF.status << 8));
    }
}

void handle_normal_key(uint8_t scan_code, bool pressed)
{
    switch(KEYMAP[scan_code]) {
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            SET_KBSTATUS(STATUS_SHIFT); break;
        case CAPSLOCK:
            if(pressed)
                KBBUF.status ^= STATUS_CAPS;
        case ALT:
            SET_KBSTATUS(STATUS_ALT); break;
        case CTRL:
            SET_KBSTATUS(STATUS_CTRL); break;
        default:
            if(pressed)
                enq_kbbuf(&KBBUF, scan_code | (KBBUF.status << 8));
    }
}

void _inthandler21()
{
    uint8_t val = inb(0x60);
    if(val == 0xE0) {       /* extended key */
        val = inb(0x60);
        if(val & 0x80) {    /* extended key released */
            handle_extended_key(val & ~0x80, false);
        } else {            /* extended key pressed */
            handle_extended_key(val, true);
        }
    } else if(val < 0xE0) { /* normal key */
        if(val & 0x80) {    /* normal key released */
            handle_normal_key(val & ~0x80, false);
        } else {            /* normal key pressed */
            handle_normal_key(val, true);
        }
    }
    pic_eoi(1);
    wake_up_event(KEYBOARD);
}

#undef SET_KBSTATUS

