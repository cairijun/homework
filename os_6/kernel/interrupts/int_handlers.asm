%macro WRAP_INT_HANDLER 1
[extern _%1]
[global %1]
%1:
    pushad
    call dword _%1
    popad
    iretd
%endmacro

global int_null_handler
int_null_handler:
    iretd

global irq_null_handler0
irq_null_handler0:
    mov al, 0x20
    out 0x20, al
    iretd

global irq_null_handler1
irq_null_handler1:
    mov al, 0x20
    out 0xA0, al
    mov al, 0x20
    out 0x20, al
    iretd

global test_int
test_int:
    pushad
    mov eax, cr2
    popad
    iretd

WRAP_INT_HANDLER inthandler20
WRAP_INT_HANDLER inthandler21
