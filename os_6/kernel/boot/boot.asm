MBALIGN     equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ  -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bootstrap_stack
align 4
stack_bottom:
times 16384 db 0
stack_top:

section .descriptor_tables
align 4
global gdt,idt,gdtr,idtr
gdt:
    dq 0x0
    dq 0x00CF9A000000FFFF ;system code
    dq 0x00CF92000000FFFF ;system data
    dq 0x00CFFA000000FFFF ;user code
    dq 0x00CFF2000000FFFF ;user data
    dq 0x00CFF2000000FFFF ;user stack for syscalls
    times 0x10000-$+gdt db 0
idt:
    times 0x800 db 0
gdtr:
    dw 0xffff
    dd gdt
idtr:
    dw 0x800-1
    dd idt

section .paging_info
global page_directory, kernel_page_table
align 4096
page_directory:
    times 0x1000 db 0
align 4096
kernel_page_table:
    times 0x1000 db 0

section .text
global _start
_start:
    mov esp, stack_top

    lgdt [gdtr]

    mov ax, 2<<3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x8:after_lgdt
after_lgdt:

    push ebx
    extern kmain
    call kmain

.hang:
    hlt
    jmp .hang
