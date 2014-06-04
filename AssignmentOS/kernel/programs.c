#include <stdint.h>
#include <stddef.h>
#include "tty.h"
#include "utils.h"
#include "scheduler.h"
#include "timer.h"
#include "sys.h"

unsigned int rng(unsigned int n)
{
    return n * 1664525 + 1013904223;
}

void print_str(const char *buf, uint8_t color)
{
    tty_write_with_color(CURRENT_TASK->tty, buf, strlen(buf), color);
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

#define sleep_test(name, color)\
void name()\
{\
    int i;\
    unsigned int n = CLOCK;\
    char time_str[8];\
    for(i = 0; i < 10; ++i) {\
        n = rng(n);\
        itos(n % 3000, time_str);\
        tty_write_with_color(CURRENT_TASK->tty, "I'm " #name " and I'll sleep for ", 26, color);\
        tty_write_with_color(CURRENT_TASK->tty, time_str, strlen(time_str), color);\
        tty_write_with_color(CURRENT_TASK->tty, " ms.\n", 5, color);\
        sleep(n % 3000);\
    }\
    tty_write_with_color(CURRENT_TASK->tty, "I'm " #name " and I'm exiting now. Bye!\n", 33, color);\
    _exit(0);\
}

sleep_test(P1, 0x8E);
sleep_test(P2, 0xF1);
sleep_test(P3, 0x3F);
sleep_test(P4, 0x5A);

void fork_test()
{
    uint16_t pid;
    char pid_str[8];
    itos(CURRENT_TASK->pid, pid_str);
    print_str("Fork test\nMy PID is ", 0xE8);
    print_str(pid_str, 0xE8);
    print_str(".\nI'm going to fork now.\n", 0xE8);

    pid = fork();
    if(pid) {
        itos(pid, pid_str);
        print_str("Fork success. I'm the parent process.\n", 0xE8);
        print_str("The PID of the child process is ", 0xE8);
        print_str(pid_str, 0xE8);
        print_str(".\nWait for the child process to exit to prevent it from becoming a zombie.\n", 0xE8);
        wait(pid);
        print_str("I'm the parent process and I'm exiting now. Bye!\n", 0xE8);
    } else {
        itos(CURRENT_TASK->ppid, pid_str);
        print_str("I'm the child process.\nMy parent is ", 0x8E);
        print_str(pid_str, 0x8E);
        print_str(".\nI'm exiting now. Bye!\n", 0x8E);
    }
    _exit(0);
}

unsigned int BUF[10];
size_t buf_size;

void semaphore_test()
{
    buf_size = 0;
    char str_buf[16];
    uint8_t child_color;
    sem_t buf_s, empty_s, full_s;
    sem_init(&buf_s, 1);
    sem_init(&empty_s, 0);
    sem_init(&full_s, 10);

    if((child_color = 0xF1, fork()) &&
            (child_color = 0x6A, fork()) &&
            (child_color = 0x3C, fork())) {    /* parent process, producer */
        unsigned int n = CLOCK;
        while(true) {
            sem_p(full_s);

            n = rng(n);
            itos(n % 1000, str_buf);
            print_str("Producer: producing, it will take ", 0x07);
            print_str(str_buf, 0x07);
            print_str(" ms.\n", 0x07);
            sleep(n % 1000);
            sem_p(buf_s);
            BUF[buf_size++] = n;
            sem_v(buf_s);
            print_str("Producer: finished.\n", 0x07);

            sem_v(empty_s);
        }
    } else {        /* child process, consumer */
        unsigned int n;
        while(true) {
            sem_p(empty_s);

            sem_p(buf_s);
            n = BUF[--buf_size] % 3219;
            sem_v(buf_s);

            sem_v(full_s);

            itos(n, str_buf);
            print_str("Consumer: got, consuming, it will take ", child_color);
            print_str(str_buf, child_color);
            print_str(" ms.\n", child_color);
            sleep(n);
            print_str("Consumer: finished.\n", child_color);
        }
    }
}
