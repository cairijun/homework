[ORG 0100h]
_start:
    pusha
    push ds

    mov ax, cs
    mov ds, ax

    mov si, header
    mov cx, header_len
print_header_loop:
    mov al, [si]
    call put_char_al
    inc si
    loop print_header_loop
    call new_line

    mov ax, 0
    mov es, ax
    mov bx, 7dbeh
    mov cx, 4
print_user_prog_loop:
    mov si, bx
print_user_prog_name_loop:
    mov al, [es:si]
    or al, 0
    jz print_next_user_prog
    call put_char_al
    inc si
    jmp print_user_prog_name_loop
print_next_user_prog:
    call new_line
    add bx, 10h
    loop print_user_prog_loop

    pop ds
    popa
    retf


put_char_al:
    push bx
    mov ah, 0eh
    mov bx, 0
    int 10h
    pop bx
    ret

new_line:
    mov al, 0ah
    call put_char_al
    mov al, 0dh
    call put_char_al
    ret

header:
    db 'Available user programs:'
header_len equ ($-header)
