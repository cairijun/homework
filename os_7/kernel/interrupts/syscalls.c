#include <stdint.h>
#include "syscalls.h"
#include "scheduler.h"

const syscall_t SYSCALL_TABLE[] = {
    NULL,
    sys_exit,
    sys_fork,
    NULL,
    NULL,
    NULL,
    NULL,
    sys_wait
};

int _syscalls()
{
    register int32_t syscall_num asm("eax"),
             p1 asm("ebx"),
             p2 asm("ecx"),
             p3 asm("edx");
    return SYSCALL_TABLE[syscall_num](p1, p2, p3);
}
