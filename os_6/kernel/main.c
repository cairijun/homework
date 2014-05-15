#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "kernel.h"
#include "scheduler.h"
#include "timer.h"
#include "interrupts.h"
#include "utils.h"
#include "mm.h"
#include "tty.h"

struct G_DESC *GDT;

struct Task_t sh_task;
struct TTY_t sh_tty, kernel_log_tty;

void test_int();

void init_variables()
{
    GDT = &gdt;
    IDT = &idt;
    PAGE_DIR = &page_directory;
    KERNEL_PT = &kernel_page_table;
}

void kmain(void *mb_info)
{
    cli();
    init_variables();
    enable_paging();
    init_mm(((uint32_t *)mb_info)[2]);
    init_idt();
    init_pic();
    init_timer();
    init_kb();
    init_scheduler();

    init_task(&sh_task, 0x8, 0x10, 0x10);
    tty_open(&sh_tty);
    sh_task.tty = &sh_tty;
    sh_task.tss.eip = (uint32_t)sh_main;
    add_task(&sh_task);

    start_scheduler();
    sti();
}
