#ifndef _KEYBOARD_MAP_H
#define _KEYBOARD_MAP_H
#include <stdint.h>

/* special keys */
enum{NONE=0xE0, ESC, ENTER, CTRL, RIGHT_CTRL, LEFT_SHIFT, RIGHT_SHIFT, ALT,
     RIGHT_ALT, CAPSLOCK, HOME, END, INSERT, DEL, PAGE_UP, PAGE_DOWN, LEFT, RIGHT, UP,
     DOWN, BACKSPACE};

extern uint16_t KEYMAP[0x80];

extern char SHIFTMAP[0x80];
#endif
