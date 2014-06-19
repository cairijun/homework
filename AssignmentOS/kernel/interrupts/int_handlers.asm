%macro WRAP_INT_HANDLER 1
[extern _%1]
[global %1]
%1:
    call dword _%1
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

extern _sys_fork
global sys_fork
sys_fork: ;not an int handler but called by an int handler
    enter 0, 0
    push gs
    push fs
    push ds
    push ss
    push cs
    push es
    push edi
    push esi
    push ebp
    push esp
    push ebx
    push edx
    push ecx
    pushfd
    push after_fork
    call _sys_fork
after_fork:
    leave
    ret

extern _page_fault
global page_fault
page_fault:
    mov eax, cr2
    push eax
    call _page_fault
    iretd

extern _syscalls
global syscalls
syscalls:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    call _syscalls
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    iretd

WRAP_INT_HANDLER inthandler20
WRAP_INT_HANDLER inthandler21
WRAP_INT_HANDLER inthandler26
