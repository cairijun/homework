#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "tty.h"

struct G_DESC
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t _FIXED_0        : 4;
    uint8_t _FIXED_1        : 1;
    uint8_t priv_level      : 2;
    uint8_t present         : 1;
    uint8_t limit_high      : 4;
    uint8_t AVL_to_sys      : 1;
    uint8_t _FIXED_2        : 1;
    uint8_t big             : 1;
    uint8_t granularity     : 1;
    uint8_t base_high;
}__attribute__((packed));

struct T_DESC
{
}__attribute__((packed));

typedef uint8_t PAGE[4096];

extern struct G_DESC gdt;
extern struct I_DESC idt;
extern uint32_t page_directory;
extern uint32_t kernel_page_table;
extern PAGE _START_OF_TEXT_;
extern PAGE _START_OF_RODATA_;
extern PAGE _START_OF_DATA_;
extern PAGE _START_OF_BSS_STACK_;
extern PAGE _END_OF_TEXT_;
extern PAGE _END_OF_RODATA_;
extern PAGE _END_OF_DATA_;
extern PAGE _END_OF_BSS_STACK_;
extern PAGE _END_OF_KERNEL_;
void int_null_handler();

extern struct G_DESC *GDT;
extern struct TTY_t kernel_log_tty;

void sh_main();

#define kernel_log(msg) do{\
    if(!kernel_log_tty.enable)\
        tty_open(&kernel_log_tty);\
    tty_write_with_color(&kernel_log_tty, #msg, strlen(msg), 0x05);\
} while(0)

#define log_success() do{\
    if(!kernel_log_tty.enable)\
        tty_open(&kernel_log_tty);\
    tty_write_with_color(&kernel_log_tty, "SUCCESS\n", 8, 0x0A);\
} while(0)

#endif
