#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_
#include<stdint.h>
#include "os.h"

typedef int32_t time_t;
typedef size_t ssize_t;

ssize_t sys_read(int, void *, size_t);
ssize_t sys_write(int, const void *, size_t);
time_t sys_time(time_t *);
void sys_exit(int);

extern int errno;
#define EBADF       9
#define EINTR       4
#define ENOSYS      38

static inline ssize_t read(int fd, void *buf, size_t len)
{
    ssize_t res;
    __asm__(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        : "=a"(res)
        : "a"(3), "b"(fd), "c"(buf), "d"(len)
        : "memory"
    );
    return res;
}

static inline ssize_t write(int fd, const void *buf, size_t len)
{
    ssize_t res;
    __asm__(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        : "=a"(res)
        : "a"(4), "b"(fd), "c"(buf), "d"(len)
        :
    );
    return res;
}

static inline time_t time(time_t *t)
{
    time_t res;
    __asm__(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        : "=a"(res)
        : "a"(13), "b"(t)
        :
    );
    return res;
}

static inline void _exit(int status)
{
    __asm__(
        ".intel_syntax noprefix;"
        "int 0x80;"
        ".att_syntax;"
        : /* no output */
        : "a"(1), "b"(status)
        :
    );
}
#endif
