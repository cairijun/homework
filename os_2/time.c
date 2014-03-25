#include "user_program.h"
#include "utils_32cc.h"

int main()
{
    short hour_minutes, seconds_ds;
    char hour, minutes, seconds, ds;

    __asm__(
        "clc;"
        "movb $2, %%ah;"
        "int $0x1a;"
        : "=c"(hour_minutes), "=d"(seconds_ds)
        : /* no input */
        : "cc", "ah"
    );

    hour = (hour_minutes & 0xff00) >> 8;
    minutes = hour_minutes & 0xff;
    seconds = (seconds_ds & 0xff00) >> 8;
    ds = seconds_ds & 1;

    write_str_current("Current time: ", 14);
    write_char(((hour & 0xf0) >> 4) + '0');
    write_char((hour & 0xf) + '0');
    write_char(':');
    write_char(((minutes & 0xf0) >> 4) + '0');
    write_char((minutes & 0xf) + '0');
    write_char(':');
    write_char(((seconds & 0xf0) >> 4) + '0');
    write_char((seconds & 0xf) + '0');
    
    if(ds)
        write_str_current(" DST\r\n", 6);
    else
        write_str_current("\r\n", 2);
}
