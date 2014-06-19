#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "tty.h"
#include "keyboard.h"
#include "keyboard_map.h"
#include "scheduler.h"
#include "interrupts.h"
#include "utils.h"

#define _V(x, y) vbuf[(x) + (y) * TTY_WIDTH]

void scoll_up(struct TTY_t *tty, uint8_t num);

bool tty_open(struct TTY_t *tty)
{
    if(tty == NULL)
        return false;
    tty_move_cursor(tty, 0, 0);
    tty->enable = 1;
    tty->echo = 1;
    return true;
}

size_t tty_write(struct TTY_t *tty, const char *buf, size_t len)
{
    return tty_write_with_color(tty, buf, len, 0x07);
}

size_t tty_write_with_color(struct TTY_t *tty, const char *buf, size_t len, uint8_t color)
{
    cli();
    const char *current = buf;
    if(!(tty->enable))
        return 0;
    while(len--) {
        if(*current != '\n')
            _V(tty->cur_x, tty->cur_y) = (color << 8) | *current;
        if(*current == '\n' || tty->cur_x == TTY_WIDTH) {
            if(tty->cur_y + 1 == TTY_HEIGHT)
                scoll_up(tty, 1);
            else
                ++tty->cur_y;
            tty->cur_x = 0;
        } else {
            ++tty->cur_x;
        }
        ++current;
    }
    tty_move_cursor(tty, tty->cur_x, tty->cur_y);
    sti();
    return current - buf;
}

size_t tty_read(struct TTY_t *tty, char *buf, size_t len)
{
    size_t i = 0;
    bool shift, caps, is_alpha, cont=true;
    uint16_t scan_code, key;
    if(!(tty->enable))
        return 0;
    while(i < len && cont) {
        scan_code = pop_kbbuf(&KBBUF);
        if(scan_code) {
            key = KEYMAP[scan_code & 0xFF];
            if((key & 0xFF) < 0xE0) {
                shift = scan_code & (STATUS_SHIFT << 8);
                caps = scan_code & (STATUS_CAPS << 8);
                is_alpha = (key & 0xFF) >= 'a' && (key & 0xFF) <= 'z';

                if((is_alpha && (shift ^ caps)) || (!is_alpha && shift && SHIFTMAP[key & 0xFF]))
                    buf[i] = SHIFTMAP[key & 0xFF];
                else
                    buf[i] = key & 0xFF;

                if(tty->echo)
                    tty_write(tty, buf + i, 1);
                ++i;
            } else switch(key) {
                case ENTER:
                    tty_write(tty, "\n", 1);
                    cont = false;
                    break;
                case BACKSPACE:
                    if(tty->cur_x && i) {
                        buf[--i] = '\0';
                        if(tty->echo) {
                            tty_move_cursor(tty, tty->cur_x - 1, tty->cur_y);
                            _V(tty->cur_x, tty->cur_y) = 0x0700;
                        }
                    }
                    break;
            }
        } else {
            wait_event(KEYBOARD);
        }
    }
    buf[i] = '\0';
    return i;
}

void tty_clear(struct TTY_t *tty)
{
    int8_t x, y;
    for(y = 0; y < TTY_HEIGHT; ++y) {
        for(x = 0; x < TTY_WIDTH; ++x)
            _V(x, y) = 0x0700;
    }
    tty_move_cursor(tty, 0, 0);
}

void scoll_up(struct TTY_t *tty, uint8_t num)
{
    cli();
    int8_t x, y;
    for(y = num; y < TTY_HEIGHT; ++y) {
        for(x = 0; x < TTY_WIDTH; ++x)
            _V(x, y - num) = _V(x, y);
    }
    for(x = 0; x < TTY_WIDTH; ++x)
        _V(x, tty->cur_y) = 0x0700 | ' ';
    sti();
}

void tty_move_cursor(struct TTY_t *tty, uint8_t x, uint8_t y)
{
    uint16_t pos = y * TTY_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    tty->cur_x = x;
    tty->cur_y = y;
}

int sys_tty_print(int _buf, int _color, int _1)
{
    if(_color == 0)
        _color = 0x07;
    return tty_write_with_color(CURRENT_TASK->tty, (const char *)_buf, strlen((const char *)_buf), _color);
}

#undef _V
