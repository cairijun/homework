[BITS 16]
extern read_disk, write_str, clear_scn, sleep
global _start

_start:
    mov ax, 07c0h
    mov ds, ax

    push 8h
    call sleep
    add sp, 2

    call clear_scn

    push word 0
    push word welcome_msg_len
    mov cx, welcome_msg
    push cx
    call write_str
    add sp, 6

    push 8h
    call sleep
    add sp, 2

    push word 0100h
    push word loading_msg_len
    mov cx, loading_msg
    push cx
    call write_str
    add sp, 6

    push ds
    mov ax, 0h
    mov ds, ax
    push word 0
    push word 1
    push word os_len
    push word 0500h
    call read_disk ; read os from disk
    add sp, 8
    pop ds

    push 10h
    call sleep
    add sp, 2

    ;mov ax, 0500h
    mov ax, 0500h
    jmp ax ;boot os

welcome_msg:
    db 'Welcome!'
welcome_msg_len equ ($-welcome_msg)

loading_msg:
    db 'Loading operating system...'
loading_msg_len equ ($-loading_msg)

os_len equ 2
