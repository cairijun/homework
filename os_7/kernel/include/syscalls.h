#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdint.h>

typedef int (*syscall_t)(int, int, int);

int sys_fork(int, int, int);
int _syscalls();
void syscalls();

#endif
