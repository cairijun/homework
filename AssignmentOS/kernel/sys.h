#ifndef _SYS_H
#define _SYS_H

#include <stdint.h>
#include <stdbool.h>

typedef void *sem_t;

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

static inline bool sem_init(sem_t *s, int val)
{
    bool result;
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        :"=a"(result)
        :"a"(0x8), "b"(s), "c"(val):
    );
    return result;
}

static inline void sem_p(sem_t s)
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        ::"a"(0x9), "b"(s):
    );
}

static inline void sem_v(sem_t s)
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        ::"a"(0xa), "b"(s):
    );
}

#endif
