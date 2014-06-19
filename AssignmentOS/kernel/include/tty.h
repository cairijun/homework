#ifndef _TTY_H
#define _TTY_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

struct TTY_t
{
    uint8_t cur_x, cur_y;
    uint16_t enable     :1;
    uint16_t echo       :1;
};

bool tty_open(struct TTY_t *tty);
size_t tty_write(struct TTY_t *tty, const char *buf, size_t len);
size_t tty_write_with_color(struct TTY_t *tty, const char *buf, size_t len, uint8_t color);
size_t tty_read(struct TTY_t *tty, char *buf, size_t len);
void tty_clear(struct TTY_t *tty);
void tty_move_cursor(struct TTY_t *tty, uint8_t x, uint8_t y);
int sys_tty_print(int _buf, int _color, int _1);

static uint16_t * const vbuf = (uint16_t *)VBUF_ADDR;

#endif
