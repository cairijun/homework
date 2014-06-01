#include "keyboard_map.h"

uint16_t KEYMAP[0x80] = {
    NONE, ESC, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', BACKSPACE, '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', ENTER, CTRL, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', LEFT_SHIFT, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', RIGHT_SHIFT, NONE,
    ALT, ' ', CAPSLOCK, NONE, NONE, NONE, NONE, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, HOME,
    UP, PAGE_UP, '-', LEFT, '5', RIGHT, '+', END,
    NONE, PAGE_DOWN, INSERT, DEL,
};

char SHIFTMAP[0x80] = {0};

