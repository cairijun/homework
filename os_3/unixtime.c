#include "user_program.h"
#include "syscalls.h"

int main()
{
    time_t timestamp = time(0);
    char buf[10];
    short n = 10;

    while(timestamp) {
        buf[--n] = timestamp % 10 + '0';
        timestamp /= 10;
    }
    write(1, buf + n, 10 - n);
}
