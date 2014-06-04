#include <stdint.h>
#include <stdbool.h>
#include "syscalls.h"
#include "constants.h"
#include "scheduler.h"
#include "interrupts.h"

struct sem_val_t SEMAPHORES[SEMAPHORE_NUM] = {{0, 0}};
const syscall_t SYSCALL_TABLE[] = {
    NULL,
    sys_exit,
    sys_fork,
    NULL,
    NULL,
    NULL,
    NULL,
    sys_wait,
    sys_sem_init,
    sys_sem_p,
    sys_sem_v,
};

int _syscalls()
{
    register int32_t syscall_num asm("eax"),
             p1 asm("ebx"),
             p2 asm("ecx"),
             p3 asm("edx");
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
}

int sys_sem_p(int _s, int _p2, int _p3)
{
    cli();
    _sem_t s = (_sem_t)_s;
    while(s->val <= 0)
        wait_event(WAIT_SEMAPHORE);
    --s->val;
    sti();
}

int sys_sem_v(int _s, int _p2, int _p3)
{
    cli();
    _sem_t s = (_sem_t)_s;
    ++s->val;
    wake_up_event(WAIT_SEMAPHORE);
    sti();
}
