#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "tty.h"
#include "mm.h"
#include "utils.h"
#include "scheduler.h"
#include "sys.h"
#include "fs.h"

void semaphore_test();
void ls();
void cat();

const char * const program_names[] = {"sem", "ls", "cat", NULL};
const void * const programs[]      = {semaphore_test, ls, cat};

void sh_write(const char *buf)
{
    tty_write(CURRENT_TASK->tty, buf, strlen(buf));
}

void sh_write_with_color(const char *buf, uint8_t color)
{
    tty_write_with_color(CURRENT_TASK->tty, buf, strlen(buf), color);
}

size_t sh_read(char *buf, size_t len)
{
    return tty_read(CURRENT_TASK->tty, buf, len);
}

void sh_clear()
{
    tty_clear(CURRENT_TASK->tty);
}

void run_program(const char *program)
{
    int i, n;
    bool background = false;
    char program_name_buf[64];
    uint16_t pid;
    strncpy(program_name_buf, program, 64);
    n = strlen(program);
    if(program[n - 1] == '&') {
        program_name_buf[n - 1] = '\0';
        background = true;
    }
    for(i = 0; program_names[i]; ++i) {
        if(strncmp(program_name_buf, program_names[i], 64) == 0) {
            if((pid = fork())) {
                if(!background) {
                    FOREGROUND_TASK = task_list[pid];
                    wait(pid);
                } else {
                    FOREGROUND_TASK = NULL;
                }
            } else {
                CURRENT_TASK->sigint_handler = sigint_default;
                __asm__ volatile(
                    "jmp *%0;"
                    ::"r"(programs[i]):
                );
            }
            return;
        }
    }

    char program_filename[70];
    int wd_len = strlen(CURRENT_TASK->working_dir);
    strncpy(program_filename, CURRENT_TASK->working_dir, 70);
    strncpy(program_filename + wd_len, program_name_buf, 70 - wd_len);

    struct File_t file;
    if(open_file_path(program_filename, &file)) {
        if(file.type != ARCHIVE) {
            sh_write_with_color(file.filename, 0x0C);
            sh_write_with_color(" is not a file.\n", 0x0C);
            return;
        }
        if((pid = fork())) {
            if(!background) {
                FOREGROUND_TASK = task_list[pid];
                wait(pid);
            } else {
                FOREGROUND_TASK = NULL;
            }
        } else {
            if(execve(program_filename) != 0)
                _exit(0);
        }
        return;
    }

    sh_write_with_color("Couldn't find program: ", 0x0C);
    sh_write_with_color(program, 0x0C);
}

void change_workdir(const char *wd)
{
    if(strncmp(wd, "./", 3) == 0)
        return;
    char wd_buf[65];
    strncpy(wd_buf, CURRENT_TASK->working_dir, 64);
    strncat(wd_buf, wd, 64);
    struct File_t tmp;
    if(open_dir_path(wd_buf, &tmp)) {
        if(tmp.type != DIRECTORY) {
            sh_write_with_color(tmp.filename, 0x0C);
            sh_write_with_color(" is not a directory.", 0x0C);
            return;
        }
        if(strncmp(wd, "../", 4) == 0) {
            *strrchr(wd_buf, '/') = '\0';
            *strrchr(wd_buf, '/') = '\0';
            *(strrchr(wd_buf, '/') + 1) = '\0';
        }
        strncpy(CURRENT_TASK->working_dir, wd_buf, 64);
    }
    else {
        sh_write_with_color("Couldn't find directory: ", 0x0C);
        sh_write_with_color(wd, 0x0C);
    }
}

const char *gpl_msg = 
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n\n"
    
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n";

void sh_main()
{
    char cmd_buf[70];
    size_t len;
    sh_clear();
    sh_write_with_color("AssignmentOS Version 1.0\n", 0x0B);
    sh_write_with_color("Copyright (C) 2014  Richard Tsai (http://richardtsai.info)\n\n", 0x0F);
    sh_write(gpl_msg);
    init_fat();
    while(true) {
        sh_write("\n");
        sh_write(CURRENT_TASK->working_dir);
        sh_write(" $ ");
        len = sh_read(cmd_buf, 64);
        cmd_buf[len] = '\0';
        if(len) {
            if(len > 3 && strncmp(cmd_buf, "cd ", 3) == 0) {
                if(cmd_buf[len - 1] != '/')
                    strncat(cmd_buf, "/", 1);
                change_workdir(cmd_buf + 3);
            }
            else
                run_program(cmd_buf);
        }
    }
}
