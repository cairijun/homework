#include "sys.h"
static inline unsigned int rng(unsigned int n)
{
    return n * 1664525 + 1013904223;
}

static inline void itos(int n, char *buf)
{
    int i = 0;
    char tmp;
    if(n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while(n) {
        buf[i++] = n % 10 + '0';
        n /= 10;
    }
    for(n = 0; n < i >> 1; ++n) {
        tmp = buf[n];
        buf[n] = buf[i - n - 1];
        buf[i - n - 1] = tmp;
    }
    buf[i] = '\0';
}

#define sleep_test(name, color)\
int main()\
{\
    int i;\
    unsigned int n = clock();\
    char time_str[8];\
    for(i = 0; i < 10; ++i) {\
        n = rng(n);\
        itos(n % 3000, time_str);\
        tty_print("I'm " #name " and I'll sleep for ", color);\
        tty_print(time_str, color);\
        tty_print(" ms.\n", color);\
        sleep(n % 3000);\
    }\
    tty_print("I'm " #name " and I'm exiting now. Bye!\n", color);\
    return 0;\
}

