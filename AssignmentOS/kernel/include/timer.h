#ifndef _TIMER_H
#define _TIMER_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "constants.h"

typedef void (*timer_handler_t)(void *);

struct Timer_t {
    timer_handler_t handler;
    void *arg;
    uint32_t target_time;
    uint32_t interval;
};

struct TimerHeap_t {
    struct Timer_t heap[TIMER_HEAP_SIZE];
    size_t size;
};

bool timer_heap_insert(struct Timer_t);
bool timer_heap_pop();
void _sleep(uint32_t msec);
int sys_sleep(int ms, int _1, int _2);
int sys_clock(int _1, int _2, int _3);

void init_timer();
void _inthandler20();
void inthandler20();

extern uint32_t CLOCK;
extern struct TimerHeap_t TIMER_HEAP;

#endif
