#include "user_program.h"
#include "utils_32cc.h"

int main()
{
    short year, month_day;

    __asm__(
        "clc;"
        "movb $4, %%ah;"
        "int $0x1a;"
        : "=c"(year), "=d"(month_day)
        : /* no input */
        : "cc", "ah"
    );

    write_str_current("Current date: ", 14);
    write_char(((year & 0xf000) >> 12) + '0');
    write_char(((year & 0xf00) >> 8) + '0');
    write_char(((year & 0xf0) >> 4) + '0');
    write_char((year & 0xf) + '0');
    write_char('-');
    write_char(((month_day & 0xf000) >> 12) + '0');
    write_char(((month_day & 0xf00) >> 8) + '0');
    write_char('-');
    write_char(((month_day & 0xf0) >> 4) + '0');
    write_char((month_day & 0xf) + '0');
    write_str_current("\r\n", 2);
}
