#include <stdint.h>
#include <stddef.h>
#include "tty.h"
#include "utils.h"
#include "scheduler.h"
#include "timer.h"
#include "sys.h"
#include "fs.h"

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

volatile unsigned int BUF[10];
volatile size_t buf_size;

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
            tty_print("Producer: producing, it will take ", 0x07);
            tty_print(str_buf, 0x07);
            tty_print(" ms.\n", 0x07);
            sleep(n % 1000);
            sem_p(buf_s);
            BUF[buf_size++] = n;
            sem_v(buf_s);
            tty_print("Producer: finished.\n", 0x07);

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
            tty_print("Consumer: got, consuming, it will take ", child_color);
            tty_print(str_buf, child_color);
            tty_print(" ms.\n", child_color);
            sleep(n);
            tty_print("Consumer: finished.\n", child_color);
        }
    }
}

char read_buf[8192];

void ls()
{
    struct File_t file;
    file.location = 0;
    file.type = DIRECTORY;
    if(!open_dir_path(CURRENT_TASK->working_dir, &file))
        _exit(-1);
    do{
        if(file.type == DIRECTORY)
            tty_print(file.filename, 0x6);
        else
            tty_print(file.filename, 0x7);
        tty_print("\n", 0x7);
    }while(get_next_file(&file));
    _exit(0);
}

void cat()
{
    char filename[64];
    strncpy(filename, CURRENT_TASK->working_dir, 64);
    size_t len = strlen(filename);
    write(1, "Filename: ", 10);
    if(read(0, filename + len, 64 - len)) {
        if((len = read_file_path(filename, read_buf))) {
            write(1, read_buf, len);
        }
        else {
            tty_print("Unable to open file ", 0x0C);
            tty_print(filename, 0x0C);
            tty_print(".\n", 0x0C);
        }
    }
    _exit(0);
}
