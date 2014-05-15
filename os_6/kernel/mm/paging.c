#include<stdint.h>
#include<stdbool.h>
#include "kernel.h"
#include "utils.h"
#include "mm.h"

void enable_paging()
{
    PAGE *addr;

    PAGE_DIR[0] = make_pde(((uint32_t)KERNEL_PT) >> 12, 0, 1);

    for(addr = 0; addr < &_START_OF_TEXT_; ++addr)
        KERNEL_PT[PT_NUM_ADDR(addr)] = make_pte(PT_NUM_ADDR(addr), 1, 0, 1);

    for(addr = &_START_OF_TEXT_; addr < &_END_OF_RODATA_; ++addr)
        KERNEL_PT[PT_NUM_ADDR(addr)] = make_pte(PT_NUM_ADDR(addr), 1, 0, 0);

    for(addr = &_START_OF_DATA_; addr < &_END_OF_BSS_STACK_; ++addr)
        KERNEL_PT[PT_NUM_ADDR(addr)] = make_pte(PT_NUM_ADDR(addr), 1, 0, 1);

    __asm__(
        ".intel_syntax noprefix;"
        "mov cr3, %0;"
        "mov eax, cr0;"
        "or eax, 0x80000000;"
        "mov cr0, eax;"
        ".att_syntax;"
        :
        :"r"(PAGE_DIR)
        :"eax"
    );
}

void map_kernel_page(uint32_t virt_page_num, uint32_t phy_page_num,
        bool global, bool user, bool read_write)
{
    uint16_t pd_idx = (virt_page_num & 0xffc00) >> 10,
             pt_idx = (virt_page_num & 0x3ff);
    uint32_t *page_table;

    if(!(PAGE_DIR[pd_idx] & 0x1)) {
        uint32_t pt_page_num = alloc_phy_page(0, 1023);
        map_kernel_page(pt_page_num, pt_page_num, true, false, true);
        memset((void *)(pt_page_num << 12), 0, 4096);
        PAGE_DIR[pd_idx] = make_pde(pt_page_num, user, read_write);
    }

    page_table = (uint32_t *)(PAGE_DIR[pd_idx] & 0xfffff000);
    page_table[pt_idx] = make_pte(phy_page_num, global, user, read_write);
    ++phy_mem_rc[phy_page_num];
}

void map_page(uint32_t *page_dir, uint32_t virt_page_num, uint32_t phy_page_num,
        bool global, bool user, bool read_write)
{
    uint16_t pd_idx = (virt_page_num & 0xffc00) >> 10,
             pt_idx = (virt_page_num & 0x3ff);
    uint32_t *page_table;

    if(!(page_dir[pd_idx] & 0x1)) {
        uint32_t pt_page_num = alloc_phy_page(0, 1024);
        map_kernel_page(pt_page_num, pt_page_num, true, false, true);
        memset((void *)(pt_page_num << 12), 0, 4096);
        page_dir[pd_idx] = make_pde(pt_page_num, user, read_write);
    }

    page_table = (uint32_t *)(page_dir[pd_idx] & 0xfffff000);
    page_table[pt_idx] = make_pte(phy_page_num, global, user, read_write);
    ++phy_mem_rc[phy_page_num];
}

uint32_t make_pde(uint32_t pt_page_num, bool user, bool read_write)
{
    uint32_t pde = 0;
    if(pt_page_num > 0xfffff)
        return 0;
    pde |= pt_page_num << 12;
    pde |= (user & 0x1) << 2;
    pde |= (read_write & 0x1) << 1;
    pde |= 0x1;
    return pde;
}

uint32_t make_pte(uint32_t page_num, bool global, bool user, bool read_write)
{
    uint32_t pte = 0;
    if(page_num > 0xfffff)
        return 0;
    pte |= page_num << 12;
    pte |= (global & 0x1) << 8;
    pte |= (user & 0x1) << 2;
    pte |= (read_write & 0x1) << 1;
    pte |= 0x1;
    return pte;
}

void free_page_structure(uint32_t *page_dir)
{
    uint16_t idx, i;
    uint32_t *page_table;
    for(idx = 1; idx < 1024; ++idx) { /* start from 1 to skip the kernel page table */
        if(page_dir[idx] & 0x1) {
            page_table = (uint32_t *)(page_dir[idx] & 0xfffff000);
            for(i = 0; i < 1024; ++i) {
                if(page_table[i] & 0x1)
                    dec_page_ref(page_table[i] >> 12);
            }
            dec_page_ref(PT_NUM_ADDR(page_table));
        }
    }
    dec_page_ref(PT_NUM_ADDR(page_dir));
}
