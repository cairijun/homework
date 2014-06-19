#include <stdint.h>
#include <stdbool.h>
#include "scheduler.h"
#include "constants.h"
#include "interrupts.h"
#include "kernel.h"
#include "timer.h"
#include "mm.h"
#include "utils.h"
#include "fs.h"

struct Task_t *CURRENT_TASK, *FOREGROUND_TASK, *task_list[MAX_PROCESS_NUM];
struct TaskQueue_t ready, blocked;
struct Task_t SchedulerTask;

void add_to_task_queue(struct TaskQueue_t *q, struct Task_t *t)
{
    cli();
    if(q->size)
        q->tail->next = t;
    else
        q->head = t;
    q->tail = t;
    ++q->size;
    t->next = NULL;
    sti();
}

void init_scheduler()
{
    CURRENT_TASK = NULL;
    FOREGROUND_TASK = NULL;
    ready.size = 0;
    ready.head = ready.tail = NULL;
    blocked.size = 0;
    blocked.head = blocked.tail = NULL;
    memset(task_list, 0, sizeof(task_list));

    struct Timer_t timer;
    timer.arg = NULL;
    timer.handler = force_switch;
    timer.interval = 10;
    timer.target_time = 0;
    timer_heap_insert(timer);
}

void start_scheduler()
{
    SchedulerTask.sigint_handler = NULL;
    SchedulerTask.tss.cr3 = (uint32_t)PAGE_DIR;
    SchedulerTask.tss.eflags = 0xE0000011;
    GDT[7].limit_low = 0x67;
    GDT[7].base_low = (uint32_t)&SchedulerTask.tss & 0xFFFF;
    GDT[7].base_mid = ((uint32_t)&SchedulerTask.tss & 0xFF0000) >> 16;
    GDT[7]._FIXED_0 = 9;
    GDT[7]._FIXED_1 = 0;
    GDT[7].priv_level = 0;
    GDT[7].present = 1;
    GDT[7].limit_high = 0;
    GDT[7].AVL_to_sys = 0;
    GDT[7]._FIXED_2 = 0;
    GDT[7].big = 0;
    GDT[7].granularity = 0;
    GDT[7].base_high = (uint32_t)&SchedulerTask.tss >> 24;
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "ltr ax;"
        ".att_syntax;"
        ::"a"(7 << 3):
    );
    scheduler();
}

void wait_event(enum Event e)
{
    CURRENT_TASK->block_event = e;
    pwait();
}

void pwait()
{
    CURRENT_TASK->status = BLOCKED;
    add_to_task_queue(&blocked, CURRENT_TASK);
    CURRENT_TASK = NULL;
    TASK_SWITCH();
}

void wake_up_event(enum Event e)
{
    cli();
    struct Task_t *curr = blocked.head, *prev = NULL;
    while(curr) {
        if(curr->block_event == e) {
            if(prev)
                prev->next = curr->next;
            else
                blocked.head = curr->next;
            if(curr->next == NULL)
                blocked.tail = prev;
            if(--blocked.size <= 1)
                blocked.tail = blocked.head;
            curr->block_event = NONBLOCKED;
            curr->status = READY;
            add_to_task_queue(&ready, curr);
            break;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    sti();
}

void wake_up_pid(uint16_t pid)
{
    cli();
    struct Task_t *curr = blocked.head, *prev = NULL;
    while(curr) {
        if(curr->pid == pid) {
            if(prev)
                prev->next = curr->next;
            else
                blocked.head = curr->next;
            if(curr->next == NULL)
                blocked.tail = prev;
            if(--blocked.size <= 1)
                blocked.tail = blocked.head;
            curr->block_event = NONBLOCKED;
            curr->status = READY;
            add_to_task_queue(&ready, curr);
            break;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    sti();
}

void scheduler()
{
    while(true) {
        cli();
        while(ready.size == 0) {
            __asm__ volatile(
                "sti;"
                "hlt;"
            );
        }
        CURRENT_TASK = ready.head;
        CURRENT_TASK->status = RUNNING;
        ready.head = CURRENT_TASK->next;
        if(--ready.size == 0)
            ready.tail = NULL;
        farjmp(0, CURRENT_TASK->pid << 3);
        sti();
    }
}

void force_switch()
{
    if(CURRENT_TASK) {
        add_to_task_queue(&ready, CURRENT_TASK);
        CURRENT_TASK->status = READY;
        TASK_SWITCH();
    }
}

void init_task(struct Task_t *t, uint16_t cs, uint16_t ds, uint16_t ss)
{
    memset(t, 0, sizeof(struct Task_t));
    uint32_t page_dir_pn = alloc_phy_page(0, 1023);
    uint32_t stack;
    t->block_event = NONBLOCKED;
    t->page_dir = (uint32_t *)(page_dir_pn << 12);
    if(CURRENT_TASK)
        t->ppid = CURRENT_TASK->pid;
    else
        t->ppid = 7;
    map_kernel_page(page_dir_pn, page_dir_pn, true, false, true);
    memset(t->page_dir, 0, 4096);
    t->page_dir[0] = PAGE_DIR[0];
    t->block_event = 0;
    t->next = NULL;
    t->status = READY;
    t->tss.cr3 = (uint32_t)t->page_dir;
    t->tss.eflags = 0x202;
    t->tss.cs = cs;
    t->tss.ds = ds;
    t->tss.es = ds;
    t->tss.fs = ds;
    t->tss.gs = ds;
    t->tss.ss = ss;

    t->sem_list_size = 0;
    t->sigint_handler = sigint_default;

    stack = alloc_phy_page(1024, max_page_num);
    map_page(t->page_dir, (0xFFFFFFFF - 4096 + 1) >> 12, stack, false, true, true);
    t->tss.esp = 0xFFFFFFFF;
}

void add_task(struct Task_t *t)
{
    uint16_t pid;
    for(pid = 8; pid < MAX_PROCESS_NUM; ++pid) {
        if(!GDT[pid].present) {
            GDT[pid].limit_low = 0x67;
            GDT[pid].base_low = (uint32_t)&t->tss & 0xFFFF;
            GDT[pid].base_mid = ((uint32_t)&t->tss & 0xFF0000) >> 16;
            GDT[pid]._FIXED_0 = 9;
            GDT[pid]._FIXED_1 = 0;
            GDT[pid].priv_level = 0;
            GDT[pid].present = 1;
            GDT[pid].limit_high = 0;
            GDT[pid].AVL_to_sys = 0;
            GDT[pid]._FIXED_2 = 0;
            GDT[pid].big = 0;
            GDT[pid].granularity = 0;
            GDT[pid].base_high = (uint32_t)&t->tss >> 24;
            t->pid = pid;
            break;
        }
    }
    task_list[pid] = t;
    add_to_task_queue(&ready, t);
}

int sys_exit(int code, int _1, int _2)
{
    struct Task_t *p_task = task_list[CURRENT_TASK->ppid];
    size_t i;
    CURRENT_TASK->exit_code = code;
    CURRENT_TASK->status = ZOMBIE;

    free_page_structure(CURRENT_TASK->page_dir);
    for(i = 0; i < CURRENT_TASK->sem_list_size; ++i)
        --CURRENT_TASK->sem_list[i]->rc;

    if(p_task && p_task->status == BLOCKED && p_task->block_event == WAIT_CHILD)
        wake_up_pid(p_task->pid);
    CURRENT_TASK = NULL;
    TASK_SWITCH();
}

int sys_wait(int pid, int _1, int _2)
{
    struct Task_t *c_task = task_list[pid];
    if(c_task == NULL)
        return -1;
    while(c_task->status != ZOMBIE) {
        CURRENT_TASK->block_event = WAIT_CHILD;
        pwait();
    }
    GDT[pid].present = 0;
    task_list[pid] = NULL;
    dec_page_ref(PT_NUM_ADDR(c_task));
    return c_task->exit_code;
}

uint16_t _sys_fork(uint32_t eip, uint32_t eflags, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esp,
        uint32_t ebp, uint32_t esi, uint32_t edi, uint32_t es, uint32_t cs, uint32_t ss, uint32_t ds,
        uint32_t fs, uint32_t gs)
{
    cli();

    uint32_t task_struct_pn = alloc_phy_page(0, 1023);
    map_page(CURRENT_TASK->page_dir, task_struct_pn, task_struct_pn, false, false, true);
    struct Task_t *t = (struct Task_t *)(task_struct_pn << 12);
    memcpy(t, CURRENT_TASK, sizeof(struct Task_t));
    t->page_dir = clone_page_structure((uint32_t *)CURRENT_TASK->tss.cr3);
    t->tss.cr3 = (uint32_t)t->page_dir;

    /* allocate a new stack and copy the orignal one */
    uint32_t stack = alloc_phy_page(1024, max_page_num);
    map_page(CURRENT_TASK->page_dir, 1024, stack, false, true, true);
    memcpy((uint32_t *)0x400000, (uint32_t *)(0xfffff000), 4096);
    map_page(t->page_dir, 0xfffff, stack, false, true, true);

    /* increase semaphores rc */
    size_t i;
    for(i = 0; i < t->sem_list_size; ++i)
        ++t->sem_list[i]->rc;

    t->tss.eip = eip;
    t->tss.eflags = eflags;
    t->tss.eax = 0;
    t->tss.ecx = ecx;
    t->tss.edx = edx;
    t->tss.ebx = ebx;
    t->tss.esp = esp;
    t->tss.ebp = ebp;
    t->tss.esi = esi;
    t->tss.edi = edi;
    t->tss.es = es;
    t->tss.cs = cs;
    t->tss.ss = ss;
    t->tss.ds = ds;
    t->tss.fs = fs;
    t->tss.gs = gs;

    t->ppid = CURRENT_TASK->pid;
    add_task(t);

    sti();
    return t->pid;
}

int sys_execve(int _filename, int _argv, int _envp)
{
    const char *filename = (const char *)_filename;
    struct File_t program;
    if(!open_file_path(filename, &program))
        return -1;

    size_t program_size = program.size, n_pages = ((program_size - 1) >> 12) + 1;
    size_t i;
    for(i = 0; i < n_pages; ++i)
        map_page(CURRENT_TASK->page_dir,
                (USERSPACE_START >> 12) + i, alloc_phy_page(1024, max_page_num),
                false, false, true);
    if(!read_file(&program, (void *)USERSPACE_START))
        return -1;
    CURRENT_TASK->sigint_handler = sigint_default;
    __asm__ volatile (
        "jmp *%0;"
        ::"r"(USERSPACE_START):
    );
    return 0;
}

void _force_exit()
{
    if(CURRENT_TASK->status == BLOCKED) {   /* remove from blocked queue */
        struct Task_t *curr = blocked.head, *prev = NULL;
        while(curr) {
            if(curr->pid == CURRENT_TASK->pid) {
                if(prev)
                    prev->next = curr->next;
                else
                    blocked.head = curr->next;
                if(curr->next == NULL)
                    blocked.tail = prev;
                if(--blocked.size <= 1)
                    blocked.tail = blocked.head;
                curr->block_event = NONBLOCKED;
                curr->status = READY;
                break;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
    sys_exit(-1, 0, 0);
}

void sigint_default()
{
    if(FOREGROUND_TASK && FOREGROUND_TASK->status != ZOMBIE) {
        CURRENT_TASK = FOREGROUND_TASK;
        CURRENT_TASK->tss.eip = (uint32_t)_force_exit;
        farjmp(0, CURRENT_TASK->pid << 3);
    }
}

int sys_getpid(int _1, int _2, int _3)
{
    return CURRENT_TASK->pid;
}

int sys_getppid(int _1, int _2, int _3)
{
    return CURRENT_TASK->ppid;
}

