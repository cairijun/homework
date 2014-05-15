#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

void inthandler21();
void _inthandler21();
void init_kb();

enum {STATUS_CTRL = 0x1, STATUS_SHIFT = 0x2, STATUS_ALT = 0x4, STATUS_CAPS = 0x8};
struct KeyboardBuf_t
{
    uint16_t buf[KB_BUFSIZE];
    uint8_t head, tail;
    uint8_t status;
};
extern struct KeyboardBuf_t KBBUF;

static inline void init_kbbuf(struct KeyboardBuf_t *kbbuf)
{
    kbbuf->status = 0;
    kbbuf->head = 0;
    kbbuf->tail = 0;
}

static inline bool enq_kbbuf(struct KeyboardBuf_t *kbbuf, uint16_t c)
{
    if((kbbuf->tail - kbbuf->head) % KB_BUFSIZE == KB_BUFSIZE - 1)
        return false;
    kbbuf->buf[kbbuf->tail] = c;
    kbbuf->tail = (kbbuf->tail + 1) % KB_BUFSIZE;
    return true;
}

static inline uint16_t front_kbbuf(struct KeyboardBuf_t *kbbuf)
{
    if(kbbuf->head == kbbuf->tail)
        return 0;
    return kbbuf->buf[kbbuf->head];
}

static inline uint16_t pop_kbbuf(struct KeyboardBuf_t *kbbuf)
{
    uint16_t val = front_kbbuf(kbbuf);
    if(val)
        kbbuf->head = (kbbuf->head + 1) % KB_BUFSIZE;
    return val;
}

#endif
