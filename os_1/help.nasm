[ORG 0100h]
_start:
    pusha
    push ds

    mov ax, cs
    mov ds, ax
    mov si, help_msg
    mov cx, help_msg_len
print_loop:
    mov al, [si]
    mov bx, 0h
    mov ah, 0eh
    int 10h
    inc si
    loop print_loop

    pop ds
    popa
    retf

help_msg:
    db "Type the command name and press 'Enter'", 0ah, 0dh,\
    "  ?       - Print this message", 0ah, 0dh,\
    "  ls      - List user programs", 0ah, 0dh,\
    "  reboot  - Reboot", 0ah, 0dh,\
    "  <names> - Run user programs (a comma seperated list)", 0ah, 0dh,\
    0ah, 0dh,\
    "    Example: ?,ls,stone<Enter>", 0ah, 0dh
help_msg_len equ ($-help_msg)
