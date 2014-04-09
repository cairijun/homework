#ifndef _USER_PROGRAM_H
#define _USER_PROGRAM_H
__asm__(".code16gcc\n");
__asm__("call main\n");
__asm__("lret\n");
#endif
