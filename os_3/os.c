__asm__(".code16gcc\n");
__asm__("mov $0, %eax\n");
__asm__("mov %ax, %ds\n");
__asm__("mov %ax, %es\n");
__asm__("jmpl $0, $main\n");

#include<stdint.h>
#include "os.h"
#include "utils_32cc.h"
#include "int_handler_wrappers.h"
#include "syscalls.h"

const char * const welcome_msg = "Welcome to the OS Operation System!\r\n"
"Copyright \xa9 Richard Tsai (github.com/richardtsai)\r\n"
"Enter 'help' to see help\r\n";
const char * const prompt = "\r\n#> ";
const int8_t cmd_col = 3;
char cmd_buf[CMD_BUF_LEN];
void *prev_esp;

int8_t wait_cmd();
int8_t load_program(const char *, uint16_t);
void run_program();
void init_spin_mark();
void _spin_mark();
void _syscall();

int main()
{
    int8_t cmd_len;
    char cmd[32];
    char * current_comma_pos, * prev_comma_pos;
    clear_scn();
    write_str(welcome_msg, __builtin_strlen(welcome_msg), 0);

    add_int_handler(0x1c, (void *)spin_mark);
    add_int_handler(0x80, (void *)syscall);

    while(1) {
        write_str_current(prompt, cmd_col + 2);
        cmd_len = wait_cmd();
        if(cmd_len) {
            prev_comma_pos = cmd_buf - 1;
            while(prev_comma_pos < cmd_buf + cmd_len) {
                current_comma_pos = strchr_(prev_comma_pos + 1, ',');
                strncpy_(cmd, prev_comma_pos + 1, current_comma_pos - prev_comma_pos - 1);
                if(load_program(cmd, current_comma_pos - prev_comma_pos - 1))
                    run_program();
                else {
                    write_str_current("Command not found: ", 19);
                    write_str_current(cmd, current_comma_pos - prev_comma_pos - 1);
                    write_str_current("\r\n", 2);
                }
                prev_comma_pos = current_comma_pos;
            }
        }
    }
}


int8_t wait_cmd()
{
    int8_t c, len = 0;
    uint16_t current_pos, i;
    while('\r' != (c = get_kb_char())) {
        if(c == '\b') {
            current_pos = get_cursor();
            if((current_pos & 0xff) > cmd_col) {
                write_char(c);
                write_char(' ');
            }
            else
                continue;
        }
        write_char(c);
    }

    current_pos = get_cursor();
    for(i = (current_pos & 0xff00) + cmd_col; i < current_pos; i +=0x1) {
        move_cursor(i);
        cmd_buf[len++] = get_char();
    }
    cmd_buf[len] = '\0';

    write_char('\r');
    write_char('\n');

    return len;
}

int8_t load_program(const char * cmd, uint16_t len)
{
    struct FILE_TABLE_ENTRY file_table[32];
    uint16_t p, i;
    for(p = 0x7dbe; p != 0x7dfe; p += 0x10) {
        read_disk((void *)file_table, 1, *(uint16_t *)p, 0);

        for(i = 0; i < 32; ++i) {
            if(strncmp_(file_table[i].filename, cmd, len) == 0) {
                read_disk((void *)0x7e00, file_table[i].size, file_table[i].loc, 0);
                return 1;
            }
        }
    }
    return 0;
}

void run_program()
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "pushad;"
        "pushf;"
        "mov %0, esp;"
        ".att_syntax;"
        : "=m"(prev_esp)
        : /* no input */
        :
    );
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "mov ax, 0x7d0;"
        "mov es, ax;"
        "mov ds, ax;"
        "mov ss, ax;"
        "mov esp, 0xffff;"
        "call 0x7d0:0x0100;"
        ".att_syntax;"
    );
    __asm__ volatile(
        ".globl run_program_cleanup;"
        ".intel_syntax noprefix;"
        "run_program_cleanup:"
        "xor ax, ax;"
        "mov ss, ax;"
        "mov ds, ax;"
        "mov es, ax;"
        "mov esp, %0;"
        "popf;"
        "popad;"
        ".att_syntax;"
        : /* no output */
        : "m"(prev_esp)
        : "ax"
    );
}

void _spin_mark()
{
    static int8_t current = 0, acc = 3;
    const char * const marks = "|/-\\|/-\\";
    uint16_t current_cursor;
    if(!--acc) {
        current_cursor = get_cursor();
        set_char(marks[current++], 0x184f);
        move_cursor(current_cursor);
        current %= 8;
        acc = 3;
    }
}

void _syscall()
{
    __asm__ volatile(
        ".intel_syntax noprefix;"
        "mov di, cs;"
        "mov ds, di;"
        "mov es, di;"
        ".att_syntax;"
        : : : "di"
    );

    register int32_t syscall_num asm("eax"),
             p1 asm("ebx"),
             p2 asm("ecx"),
             p3 asm("edx");
    if(syscall_num <= 0 || syscall_num > 13) {
        errno = ENOSYS;
        return;
    }

    errno = 0;
    switch(syscall_num)
    {
        case 1:
            sys_exit(p1); break;
        case 3:
            syscall_num = sys_read(p1, (void *)p2, p3); break;
        case 4:
            syscall_num = sys_write(p1, (void *)p2, p3); break;
        case 13:
            syscall_num = sys_time((time_t *)p1); break;
        default:
            errno = ENOSYS;
    }
}
