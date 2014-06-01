#ifndef _SYS_H
#define _SYS_H

#include <stdint.h>

static inline void _exit(uint16_t code)
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "and ebx, 0xffff;"
        "int 0x80;"
        ".att_syntax;"
        ::"a"(0x1), "b"(code):
    );
}

static inline uint16_t wait(uint16_t pid)
{
    uint16_t exit_code;
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "and ebx, 0xffff;"
        "int 0x80;"
        ".att_syntax;"
        :"=a"(exit_code)
        :"a"(0x7), "b"(pid):
    );
    return exit_code;
}

static inline uint16_t fork()
{
    uint16_t val;
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        :"=a"(val)
        :"a"(0x2):
    );
    return val;
}

#endif
