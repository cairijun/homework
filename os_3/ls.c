#include "user_program.h"
#include "utils_32cc.h"

struct FILE_TABLE_ENTRY {
    const char filename[14], loc, size;
} __attribute__ ((packed));

int main()
{
    struct FILE_TABLE_ENTRY file_table[32];
    unsigned short p, i;

    read_disk((void *)file_table, 1, 0x40, 0);
    for(i = 0; i < 32; ++i) {
        if(file_table[i].size) {
            write_str_current(file_table[i].filename, __builtin_strlen(file_table[i].filename));
            write_str_current("\r\n", 2);
        }
    }
}
