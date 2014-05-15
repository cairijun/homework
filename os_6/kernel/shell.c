#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "tty.h"
#include "mm.h"
#include "utils.h"
#include "scheduler.h"

void P1();
void P2();
void P3();
void P4();

const char * const program_names[] = {"p1", "p2", "p3", "p4", NULL};
const void * const programs[]      = {P1, P2, P3, P4};

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
    strncpy(program_name_buf, program, 64);
    n = strlen(program);
    if(program[n - 1] == '&') {
        program_name_buf[n - 1] = '\0';
        background = true;
    }
    for(i = 0; program_names[i]; ++i) {
        if(strncmp(program_name_buf, program_names[i], 64) == 0) {
            uint32_t task_struct_pn = alloc_phy_page(0, 1023);
            map_page(CURRENT_TASK->page_dir, task_struct_pn, task_struct_pn, false, true, true);
            struct Task_t *t = (struct Task_t *)(task_struct_pn << 12);
            init_task(t, 0x8, 0x10, 0x10);
            t->tty = CURRENT_TASK->tty;
            t->tss.eip = (uint32_t)programs[i];
            add_task(t);
            if(!background)
                sys_wait(t->pid);
            return;
        }
    }
    sh_write_with_color("Couldn't find program: ", 0x0C);
    sh_write_with_color(program, 0x0C);
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
    char cmd_buf[65];
    size_t len;
    sh_clear();
    sh_write_with_color("AssignmentOS Version 1.0\n", 0x0B);
    sh_write_with_color("Copyright (C) 2014  Richard Tsai (http://richardtsai.info)\n\n", 0x0F);
    sh_write(gpl_msg);
    while(true) {
        sh_write("\n/fd0/ $ ");
        len = sh_read(cmd_buf, 64);
        cmd_buf[len] = '\0';
        if(len)
            run_program(cmd_buf);
    }
}
