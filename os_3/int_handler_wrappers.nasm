[BITS 16]
%macro WRAP_INT_HANDLER 1
[extern _%1]
[global %1]
%1:
    call dword _%1
    iret
%endmacro

WRAP_INT_HANDLER spin_mark
WRAP_INT_HANDLER syscall
