#include "kernel.h"
#include "mm.h"

uint32_t *PAGE_DIR;
uint32_t *KERNEL_PT;

uint32_t kernel_brk;
uint32_t max_page_num;
uint8_t *phy_mem_rc;

void init_mm(uint32_t max_phy_byte)
{
    size_t mem_map_size;
    uint32_t i, j;

    max_page_num = max_phy_byte >> 2;
    mem_map_size = max_page_num * sizeof(uint8_t);

    kernel_brk = (PT_NUM_ADDR(&_END_OF_KERNEL_) + 1) << 12;
    phy_mem_rc = (uint8_t *)kernel_brk;

    j = (kernel_brk + mem_map_size - 1) >> 12;
    for(i = kernel_brk >> 12; i <= j; ++i)
        map_kernel_page(i, i, 1, 0, 1);
    kernel_brk += mem_map_size;

    for(i = 0; i <= j; ++i)
        phy_mem_rc[i] = 1;
    for(; j <= max_page_num; ++j)
        phy_mem_rc[j] = 0;
}

uint32_t alloc_phy_page(uint32_t from, uint32_t to)
{
    uint32_t pn = from;
    while(pn <= to && phy_mem_rc[pn])
        ++pn;
    ++phy_mem_rc[pn];
    return pn;
}

void dec_page_ref(uint32_t page_num)
{
    if(phy_mem_rc[page_num])
        --phy_mem_rc[page_num];
}
