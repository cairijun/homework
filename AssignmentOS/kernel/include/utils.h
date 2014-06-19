#ifndef _UTILS_H
#define _UTILS_H
#include<stdint.h>
#include<stddef.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "in al, dx;"
        ".att_syntax;"
        : "=a"(val)
        : "d"(port)
        :
    );
    return val;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t val;
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "in ax, dx;"
        ".att_syntax;"
        : "=a"(val)
        : "d"(port)
        :
    );
    return val;
}

static inline uint32_t ind(uint16_t port)
{
    uint32_t val;
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "in eax, dx;"
        ".att_syntax;"
        : "=a"(val)
        : "d"(port)
        :
    );
    return val;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "out dx, al;"
        ".att_syntax;"
        : /* no output */
        : "d"(port), "a"(val)
        :
    );
}

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "out dx, ax;"
        ".att_syntax;"
        : /* no output */
        : "d"(port), "a"(val)
        :
    );
}

static inline void outd(uint16_t port, uint32_t val)
{
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "out dx, eax;"
        ".att_syntax;"
        : /* no output */
        : "d"(port), "a"(val)
        :
    );
}

static inline void io_wait()
{
    __asm__ volatile (
        ".intel_syntax noprefix;"
        "out 0x80, al;"
        ".att_syntax;"
    );
}

static inline void hlt()
{
    __asm__ volatile ("hlt;");
}

void farjmp(uint32_t eip, uint32_t cs);
void farcall(uint32_t eip, uint32_t cs);
void * memset(void *, int, size_t);
uint16_t get_tr();
size_t strlen(const char *);
char * strncpy(char *, const char *, size_t);
int strncmp(const char *, const char *, size_t);
void *memcpy(void *, const void *, size_t);
char *strchr(const char *str, int chr);
char *strrchr(const char *str, int chr);
int toupper(int c);
int tolower(int c);

static inline char *strncat(char *dest, const char *src, size_t len)
{
    char *p = dest;
    p += strlen(p);
    while(len--)
        if(*src)
            *p++ = *src++;
        else
            break;
    *p = '\0';
    return dest;
}
#endif
