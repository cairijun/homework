#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdint.h>
#include <stdbool.h>

typedef int (*syscall_t)(int, int, int);
struct sem_val_t
{
    int val;
    int rc;
};
typedef struct sem_val_t *_sem_t;

int sys_fork(int, int, int);
int _syscalls();
void syscalls();

extern struct sem_val_t SEMAPHORES[];

int sys_sem_init(int s, int val, int _p3);
int sys_sem_p(int _s, int _p2, int _p3);
int sys_sem_v(int  s, int _p2, int _p3);

#endif
