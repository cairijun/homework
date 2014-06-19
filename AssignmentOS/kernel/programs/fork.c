#include "sys.h"

unsigned int rng(unsigned int n)
{
    return n * 1664525 + 1013904223;
}

void itos(int n, char *buf)
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

int main()
{
    uint16_t pid;
    char pid_str[8];
    itos(getpid(), pid_str);
    tty_print("Fork test\nMy PID is ", 0xE8);
    tty_print(pid_str, 0xE8);
    tty_print(".\nI'm going to fork now.\n", 0xE8);

    pid = fork();
    if(pid) {
        itos(pid, pid_str);
        tty_print("Fork success. I'm the parent process.\n", 0xE8);
        tty_print("The PID of the child process is ", 0xE8);
        tty_print(pid_str, 0xE8);
        tty_print(".\nWait for the child process to exit to prevent it from becoming a zombie.\n", 0xE8);
        wait(pid);
        tty_print("I'm the parent process and I'm exiting now. Bye!\n", 0xE8);
    } else {
        itos(getppid(), pid_str);
        tty_print("I'm the child process.\nMy parent is ", 0x8E);
        tty_print(pid_str, 0x8E);
        tty_print(".\nI'm exiting now. Bye!\n", 0x8E);
    }
    return 0;
}
