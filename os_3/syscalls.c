__asm__(".code16gcc\n");
#include "syscalls.h"
#include "utils_32cc.h"
#include "os.h"
#define USER_SPACE_START __asm__ volatile\
    (".intel_syntax noprefix;mov di, 0x7d0;mov ds, di;.att_syntax;":::"di")
#define USER_SPACE_END __asm__ volatile(".intel_syntax noprefix;xor di, di;mov ds, di;.att_syntax;":::"di")
int errno;

static ssize_t read_from_stdin(void *buf, size_t len);
static ssize_t write_to_stdout(const void *buf, size_t len);
static time_t mktime(int16_t year0, int8_t mon0, int8_t day, int8_t hour, int8_t min, int8_t sec);

ssize_t sys_read(int fd, void *buf, size_t len)
{
    if(fd != 0) {
        errno = EBADF;
        return -1;
    }
    return read_from_stdin(buf, len);
}

ssize_t sys_write(int fd, const void *buf, size_t len)
{
    if(fd != 1 && fd != 2) {
        errno = EBADF;
        return -1;
    }
    return write_to_stdout(buf, len);
}

time_t sys_time(time_t *t)
{
    int16_t hour_minutes, seconds_ds, cen_year, month_day, year;
    int8_t hour, minutes, seconds, month, day, dst;
    time_t res;
    __asm__(
        "clc;"
        "movb $4, %%ah;"
        "int $0x1a;"
        : "=c"(cen_year), "=d"(month_day)
        : /* no input */
        : "cc", "ah"
    );
    __asm__(
        "clc;"
        "movb $2, %%ah;"
        "int $0x1a;"
        : "=c"(hour_minutes), "=d"(seconds_ds)
        : /* no input */
        : "cc", "ah"
    );

    year = ((cen_year & 0xf000) >> 12) * 1000
        + ((cen_year & 0x0f00) >> 8) * 100
        + ((cen_year & 0x00f0) >> 4) * 10
        + ((cen_year & 0x000f));
    month = ((month_day & 0xf000) >> 12) * 10
        + ((month_day & 0x0f00) >> 8);
    day = ((month_day & 0x00f0) >> 4) * 10
        + ((month_day & 0x000f));
    hour = ((hour_minutes & 0xf000) >> 12) * 10
        + ((hour_minutes & 0x0f00) >> 8);
    minutes = ((hour_minutes & 0x00f0) >> 4) * 10
        + ((hour_minutes & 0x000f));
    seconds = ((seconds_ds & 0xf000) >> 12) * 10
        + ((seconds_ds & 0x0f00) >> 8);
    dst = seconds_ds & 1;

    res = mktime(year, month, day, hour, minutes, seconds);
    USER_SPACE_START;
    if(t)
        *t = res;
    USER_SPACE_END;
    return res;
}

void sys_exit(int status)
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "jmp 0:run_program_cleanup;"
        ".att_syntax;"
    );
}


static ssize_t read_from_stdin(void *buf, size_t len)
{
    ssize_t count = 0;
    char *_buf = (char *)buf, tmp;
    USER_SPACE_START;
    while(len--) {
        if(0x04 == (tmp = get_kb_char()))  /* EOT */
            break;
        if(tmp == '\r')
            tmp = '\n';
        *(_buf++) = tmp;
        ++count;
    }
    USER_SPACE_END;
    return count;
}

static ssize_t write_to_stdout(const void *buf, size_t len)
{
    ssize_t count = 0;
    const char *_buf = (const char *)buf;
    USER_SPACE_START;
    while(len--) {
        if(*_buf == '\n')
            write_char('\r');
        write_char(*(_buf++));
        ++count;
    }
    USER_SPACE_END;
    return count;
}


/* Convert a Gregorian date to UNIX time. The algorithm is
 * stolen from Linux kernel. Torvalds said the algorithm
 * was first published by Gauss.
 */
static time_t mktime(int16_t year0, int8_t mon0, int8_t day,
        int8_t hour, int8_t min, int8_t sec)
{
    int16_t year = year0;
    int8_t mon = mon0 - 2;
    if(mon <= 0) {
        mon += 12;
        --year;
    }
    return ((((year/4 - year/100 + year/400 + 367*mon/12 + day) + year*365 - 719499
             )*24 + hour /* now have hours */
            )*60 + min /* now have minutes */
           )*60 + sec; /* finally seconds */
}
