#ifndef _SYS_H
#define _SYS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void *sem_t;

static inline int _exit(uint16_t code)
{
    int val;
    __asm__ volatile(
        "movl $0x1, %%eax;"
        "movzxw %1, %%ebx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(code)
        :"ebx"
    );
    return val;
}

static inline uint16_t wait(uint16_t pid)
{
    uint16_t exit_code;
    __asm__ volatile(
        "movl $0x7, %%eax;"
        "movzxw %1, %%ebx;"
        "int $0x80;"
        :"=a"(exit_code)
        :"m"(pid)
        :"ebx"
    );
    return exit_code;
}

static inline uint16_t fork()
{
    uint16_t val;
    __asm__ volatile(
        "movl $0x2, %%eax;"
        "int $0x80;"
        :"=a"(val)::
    );
    return val;
}

static inline bool sem_init(sem_t *s, int val)
{
    bool result;
    __asm__ volatile(
        "movl $0x8, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "int $0x80;"
        :"=a"(result)
        :"m"(s), "m"(val)
        :"ebx", "ecx"
    );
    return result;
}

static inline int sem_p(sem_t s)
{
    int val;
    __asm__ volatile(
        "movl $0x9, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(s)
        :"ebx"
    );
    return val;
}

static inline int sem_v(sem_t s)
{
    int val;
    __asm__ volatile(
        "movl $0xa, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(s)
        :"ebx"
    );
    return val;
}

static inline size_t read(unsigned int fd, char *buf, size_t len)
{
    int val;
    __asm__ volatile(
        "movl $0x3, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "movl %3, %%edx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(fd), "m"(buf), "m"(len)
        :"ebx", "ecx", "edx"
    );
    return val;
}

static inline size_t write(unsigned int fd, const char *buf, size_t len)
{
    int val;
    __asm__ volatile(
        "movl $0x4, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "movl %3, %%edx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(fd), "m"(buf), "m"(len)
        :"ebx", "ecx", "edx"
    );
    return val;
}

static inline int execve(const char *filename)
{
    int val;
    __asm__ volatile(
        "movl $0xb, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(filename)
        :"ebx"
    );
    return val;
}

static inline uint32_t clock()
{
    uint32_t val;
    __asm__ volatile(
        "movl $0xd, %%eax;"
        "int $0x80;"
        :"=a"(val)::
    );
    return val;
}

static inline void sleep(uint32_t ms)
{
    __asm__ volatile(
        "movl $0xe, %%eax;"
        "movl %0, %%ebx;"
        "int $0x80;"
        ::"m"(ms):"ebx", "eax"
    );
}

static inline size_t tty_print(const char *buf, uint8_t color)
{
    size_t val;
    __asm__ volatile(
        "movl $0xf, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "int $0x80;"
        :"=a"(val)
        :"m"(buf), "m"(color)
        :"ebx", "ecx"
    );
    return val;
}

static inline uint16_t getpid()
{
    uint16_t val;
    __asm__ volatile(
        "movl $0x14, %%eax;"
        "int $0x80;"
        :"=a"(val)::
    );
    return val;
}

static inline uint16_t getppid()
{
    uint16_t val;
    __asm__ volatile(
        "movl $0x15, %%eax;"
        "int $0x80;"
        :"=a"(val)::
    );
    return val;
}

#endif
