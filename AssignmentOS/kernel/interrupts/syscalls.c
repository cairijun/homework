#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "syscalls.h"
#include "constants.h"
#include "scheduler.h"
#include "interrupts.h"
#include "timer.h"
#include "tty.h"

struct sem_val_t SEMAPHORES[SEMAPHORE_NUM] = {{0, 0}};
const syscall_t SYSCALL_TABLE[] = {
    NULL,
    sys_exit,
    sys_fork,
    sys_read,
    sys_write,
    NULL,
    NULL,
    sys_wait,           /* 0x7 */
    sys_sem_init,
    sys_sem_p,
    sys_sem_v,
    sys_execve,
    NULL,
    sys_clock,
    sys_sleep,
    sys_tty_print,      /* 0xF */
    NULL,
    NULL,
    NULL,
    NULL,
    sys_getpid,
    sys_getppid,
};

int _syscalls()
{
    register int32_t syscall_num asm("eax");
    int32_t p1, p2, p3;
    __asm__ volatile (
        "movl %%ebx, %0;"
        "movl %%ecx, %1;"
        "movl %%edx, %2;"
        ::"m"(p1), "m"(p2), "m"(p3):
    );
    return SYSCALL_TABLE[syscall_num](p1, p2, p3);
}

int sys_sem_init(int s, int val, int _p3)
{
    if(CURRENT_TASK->sem_list_size == SEM_PER_PROCESS)
        return false;
    size_t i;
    for(i = 0; i < SEMAPHORE_NUM; ++i) {
        if(SEMAPHORES[i].rc == 0) {
            ++SEMAPHORES[i].rc;
            SEMAPHORES[i].val = val;
            *(_sem_t *)s = &SEMAPHORES[i];
            CURRENT_TASK->sem_list[CURRENT_TASK->sem_list_size++] = *(_sem_t *)s;
            return true;
        }
    }
    return false;
}

int sys_sem_p(int _s, int _p2, int _p3)
{
    cli();
    _sem_t s = (_sem_t)_s;
    while(s->val <= 0)
        wait_event(WAIT_SEMAPHORE);
    --s->val;
    sti();
    return true;
}

int sys_sem_v(int _s, int _p2, int _p3)
{
    cli();
    _sem_t s = (_sem_t)_s;
    ++s->val;
    wake_up_event(WAIT_SEMAPHORE);
    sti();
    return true;
}

int sys_read(int fd, int buf, int count)
{
    if(fd == 0)
        return tty_read((struct TTY_t *)CURRENT_TASK->tty, (char *)buf, count);
    return 0;
}

int sys_write(int fd, int buf, int count)
{
    if(fd == 1)
        return tty_write((struct TTY_t *)CURRENT_TASK->tty, (const char *)buf, count);
    if(fd == 2)
        return tty_write_with_color((struct TTY_t *)CURRENT_TASK->tty, (const char *)buf, count, 0x0C);
    return 0;
}

