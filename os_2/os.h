#ifndef _OS_H_
#define _OS_H_
__asm__(".code16gcc\n");
__asm__("mov $0, %eax\n");
__asm__("mov %ax, %ds\n");
__asm__("mov %ax, %es\n");
__asm__("jmpl $0, $main\n");

#define CMD_BUF_LEN 64

struct FILE_TABLE_ENTRY {
    const char filename[14], loc, size;
} __attribute__ ((packed));

#endif
