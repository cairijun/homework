#include<stdint.h>
#include<stdbool.h>
#include "kernel.h"
#include "utils.h"
#include "mm.h"
#include "scheduler.h"

#define PD_IDX(pn) (((pn) & 0xffc00) >> 10)
#define PT_IDX(pn) ((pn) & 0x3ff)

uint32_t PAGE_CPY_TMP[1024];

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
        "or eax, 0x80010000;"
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
    uint16_t pd_idx = PD_IDX(virt_page_num),
             pt_idx = PT_IDX(virt_page_num);
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
    uint16_t pd_idx = PD_IDX(virt_page_num),
             pt_idx = PT_IDX(virt_page_num);
    uint32_t *page_table;

    if(!(page_dir[pd_idx] & 0x1)) {
        uint32_t pt_page_num = alloc_phy_page(0, 1023);
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

uint32_t *clone_page_structure(uint32_t *src)
{
    uint32_t *page_dir, idx, i, *src_page_table;
    page_dir = (uint32_t *)(alloc_phy_page(0, 1023) << 12);
    map_kernel_page(PT_NUM_ADDR(page_dir), PT_NUM_ADDR(page_dir), true, false, true);
    memset(page_dir, 0, 4096);
    page_dir[0] = PAGE_DIR[0];
    for(idx = 1; idx < 1023; ++idx) { /* skip kernel space and stack space */
        if(src[idx] & 0x1) {
            src_page_table = (uint32_t *)(src[idx] & 0xfffff000);
            for(i = 0; i < 1024; ++i) {
                if(src_page_table[i] & 0x1) {
                    map_page(page_dir, (idx << 10) | i, src_page_table[i] >> 12,
                            (src_page_table[i] & (1 << 8)) != 0,
                            (src_page_table[i] & (1 << 2)) != 0, false); /* WP */
                    src_page_table[i] &= ~0x2; /* WP */
                    ++phy_mem_rc[src_page_table[i] >> 12];
                }
            }
        }
    }
    /* flush TLB */
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "mov eax, cr3;"
        "mov cr3, eax;"
        ".att_syntax;"
        :::"eax"
    );
    return page_dir;
}

void _page_fault(uint32_t addr, uint32_t errcode)
{
    bool present = errcode & 0x1, write = errcode & 0x2,
         user = errcode & 0x4, insfetch = errcode & 0x16;
    if(present && write) {  /* copy-on-write */
        uint32_t *page_table;
        uint32_t vir_pn, phy_pn;
        vir_pn = PT_NUM_ADDR(addr);
        page_table = (uint32_t *)(CURRENT_TASK->page_dir[PD_IDX(vir_pn)] & 0xfffff000);
        phy_pn = page_table[PT_IDX(vir_pn)] >> 12;
        if(phy_mem_rc[phy_pn] <= 1) {
            page_table[PT_IDX(vir_pn)] |= 0x2;  /* write enable */
        } else {
            memcpy(PAGE_CPY_TMP, (void *)addr, 4096);
            dec_page_ref(phy_pn);
            phy_pn = alloc_phy_page(1024, max_page_num);
            page_table[PT_IDX(vir_pn)] &= 0xfff;
            page_table[PT_IDX(vir_pn)] |= phy_pn << 12;
            memcpy((void *)addr, PAGE_CPY_TMP, 4096);
        }
    }
}
