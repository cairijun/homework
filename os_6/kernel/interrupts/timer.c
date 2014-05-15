#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "timer.h"
#include "utils.h"
#include "scheduler.h"
#include "interrupts.h"
#include "constants.h"

#define _W(i) TIMER_HEAP.heap[i].target_time
#define _SWAP(i, j) do {\
    struct Timer_t tmp = TIMER_HEAP.heap[i];\
    TIMER_HEAP.heap[i] = TIMER_HEAP.heap[j];\
    TIMER_HEAP.heap[j] = tmp;\
} while(0)

uint32_t CLOCK;
struct TimerHeap_t TIMER_HEAP;

void timer_heap_bubble_up(size_t idx)
{
    if(idx) {
        size_t p_idx = (idx - 1) >> 1;
        if(_W(idx) < _W(p_idx)) {
            _SWAP(idx, p_idx);
            timer_heap_bubble_up(p_idx);
        }
    }
}

void timer_heap_bubble_down(size_t idx)
{
    size_t lc_idx = (idx << 1) + 1, rc_idx = (idx + 1) << 1;
    if(rc_idx < TIMER_HEAP.size) {                  /* both children exist */
        if(_W(lc_idx) < _W(idx) && _W(lc_idx) <= _W(rc_idx)) {
            _SWAP(lc_idx, idx);
            timer_heap_bubble_down(lc_idx);
        } else if(_W(rc_idx) < _W(idx) && _W(rc_idx) < _W(lc_idx)) {
            _SWAP(rc_idx, idx);
            timer_heap_bubble_down(rc_idx);
        }
    } else if(lc_idx < TIMER_HEAP.size) {           /* only left child exists */
        if(_W(lc_idx) < _W(idx)) {
            _SWAP(lc_idx, idx);  /* left child must be a leaf */
        }
    }
}

bool timer_heap_insert(struct Timer_t timer)
{
    cli();
    if(TIMER_HEAP.size >= TIMER_HEAP_SIZE)
        return false;
    TIMER_HEAP.heap[TIMER_HEAP.size] = timer;
    timer_heap_bubble_up(TIMER_HEAP.size++);
    sti();
    return true;
}

bool timer_heap_pop()
{
    cli();
    if(TIMER_HEAP.size == 0)
        return false;
    TIMER_HEAP.heap[0] = TIMER_HEAP.heap[--TIMER_HEAP.size];
    timer_heap_bubble_down(0);
    sti();
    return true;
}

void init_timer()
{
    CLOCK = 0;
    TIMER_HEAP.size = 0;

    outb(0x43, 0x34);
    outb(0x40, 0x9c);
    outb(0x40, 0x2e);

    IDT[0x20] = make_int_desc(8, (uint32_t)inthandler20);
    enable_irq(0);
}

void _inthandler20()
{
    pic_eoi(0);
    ++CLOCK;
    while(TIMER_HEAP.size && _W(0) <= CLOCK) {
        struct Timer_t tmp = TIMER_HEAP.heap[0];
        timer_heap_pop();
        if(tmp.interval) {
            tmp.target_time = CLOCK + tmp.interval;
            timer_heap_insert(tmp);
        }
        tmp.handler(tmp.arg);
    }
}

void timer_wakeup(uint16_t *pid_p)
{
    wake_up_pid(*pid_p);
}

void sleep(uint32_t msec)
{
    struct Timer_t t;
    t.arg = &CURRENT_TASK->pid;
    t.handler = (timer_handler_t)timer_wakeup;
    t.interval = 0;
    t.target_time = CLOCK + msec / 10;
    timer_heap_insert(t);
    wait();
}

#undef _W
#undef _SWAP
