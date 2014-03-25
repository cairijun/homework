#include "user_program.h"
#include "utils_32cc.h"

int main()
{
    const char *msg =
        "Type the command name and press 'Enter'\r\n"
        "  help     - Print this message\r\n"
        "  ls       - List user programs\r\n"
        "  time     - Print current time\r\n"
        "  date     - Print current date\r\n"
        "  reboot   - Reboot\r\n"
        "  <names>  - Run user programs (a comma seperated list)\r\n"
        "Example: ls,date,time<Enter>\r\n";
    write_str_current(msg, __builtin_strlen(msg));
}
