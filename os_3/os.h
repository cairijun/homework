#ifndef _OS_H_
#define _OS_H_

#include<stdint.h>

#define CMD_BUF_LEN 64

struct FILE_TABLE_ENTRY {
    const int8_t filename[14], loc, size;
} __attribute__ ((packed));

typedef uint32_t size_t;

extern void *prev_esp;

#endif
