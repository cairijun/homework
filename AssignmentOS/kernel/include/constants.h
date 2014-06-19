#ifndef _CONSTANCES_H
#define _CONSTANCES_H

#define KB_BUFSIZE      32
#define VBUF_ADDR       0xB8000
#define TTY_WIDTH       80
#define TTY_HEIGHT      25

#define TIMER_HEAP_SIZE 256

#define MAX_PROCESS_NUM 128

#define SEMAPHORE_NUM   256
#define SEM_PER_PROCESS 16

#define FDC_TIMEOUT     10000

#define USERSPACE_START 0x40000000

#endif
