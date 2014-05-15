#ifndef _MM_H
#define _MM_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kernel.h"

#define PT_NUM_ADDR(addr) (((uint32_t)addr) >> 12)

void enable_paging();
void init_mm(uint32_t max_phy_byte);
uint32_t alloc_phy_page(uint32_t from, uint32_t to);
void map_kernel_page(uint32_t virt_page_num, uint32_t phy_page_num,
        bool global, bool user, bool read_write);
void map_page(uint32_t *page_dir, uint32_t virt_page_num, uint32_t phy_page_num,
        bool global, bool user, bool read_write);

uint32_t make_pde(uint32_t pt_page_num, bool user, bool read_write);
uint32_t make_pte(uint32_t page_num, bool global, bool user, bool read_write);
void free_page_structure(uint32_t *page_dir);
void dec_page_ref(uint32_t page_num);

extern uint32_t kernel_brk;
extern uint32_t max_page_num;
extern uint8_t *phy_mem_rc;

extern uint32_t *PAGE_DIR;
extern uint32_t *KERNEL_PT;

#endif
