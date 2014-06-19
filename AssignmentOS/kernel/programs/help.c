#include "sys.h"

const char *msg = 
    "Type a command or program name and press ENTER.\n"
    "The following commands are built into the shell program.\n"
    "  cd <DIR>   Change the current directory to <DIR>. E.g.: cd boot\n"
    "  ls         List the files in the current directory.\n"
    "  cat        Print a plain text file.\n"
    "  sem        The semaphore test program.\n"
    "Some user programs should be shipped with the floppy image.\n"
    "Use the ls command to find them.\n";

int main()
{
    tty_print(msg, 0x07);
    return 0;
}
