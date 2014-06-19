int main();
__asm__ (
    "call main;"
    "movl %eax, %ebx;"
    "movl $0x1, %eax;"
    "int $0x80;"
);
