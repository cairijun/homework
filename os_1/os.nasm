[BITS 16]
extern write_str, clear_scn, get_kb_char, write_char, get_char, get_cursor
extern move_cursor, read_disk
global _start

_start:
    mov ax, 050h
    mov ds, ax

    call clear_scn

    push word 0
    push word welcome_msg_len
    mov cx, welcome_msg
    push cx
    call write_str
    add sp, 6

    jmp console_loop_call_prompt

new_line:
    push word 0ah
    call write_char
    add sp, 2
    push word 0dh
    call write_char
    add sp, 2
    ret

prompt:
    enter 0, 0
    call new_line
    mov si, console_prompt
prompt_loop:
    push word [si]  ; upper byte will be ignore
    call write_char
    add sp, 2
    inc si
    or byte [si], 0  ; check end of string
    jnz prompt_loop
    leave
    ret


console_loop_call_prompt:
    call prompt
console_loop:
    call get_kb_char
    cmp al, 0dh  ; check CR
    je console_loop_enter

    push ax
    call write_char
    pop ax

    cmp al, 8h
    jne console_loop
    push word ' '
    call write_char
    pop cx
    push word 8h
    call write_char
    pop cx

    jmp console_loop
console_loop_enter:
    call read_cmd
    or al, 0  ; check zero length cmd
    jz console_loop_call_prompt
    mov ax, cmd_buf
get_and_run_cmd_loop:
    push ax
    call get_cmd_in_buf
    pop bx
    push ax
    call new_line
    call dispatch_cmd
    pop ax
    or ax, 0
    jz get_and_run_cmd_loop_exit
    ;mov bx, ax
    jmp get_and_run_cmd_loop
get_and_run_cmd_loop_exit:
    jmp console_loop_call_prompt


read_cmd:
    mov si, cmd_buf
    mov bl, console_prompt_len
read_cmd_loop:
    call get_cursor
    mov al, bl
    push ax
    call move_cursor
    pop ax
    call get_char
    cmp al, ' '
    je read_cmd_exit
    mov [si], al
    inc si
    inc bl
    jmp read_cmd_loop
read_cmd_exit:
    mov byte [si], 0
    sub bl, console_prompt_len
    mov al, bl
    ret


dispatch_cmd:
    enter 0, 0
    mov si, 1aeh
try_pt:
    add si, 10h
    cmp si, 1feh
    je cmd_not_found

    push si
    call check_user_program
    pop si
    or ax, 0h
    jz try_pt

    movzx bx, al  ; Starting LBA
    movzx ax, ah  ; Number of sectors
    push ds
    mov cx, 0h
    mov ds, cx
    push word 0h
    push bx
    push ax
    push word 7e00h
    call read_disk
    add sp, 8
    pop ds

    call 7d0h:0100h  ; Call user program

    leave
    ret
cmd_not_found:
    call get_cursor
    mov al, 0h
    push ax
    push word cmd_not_found_msg_len
    mov cx, cmd_not_found_msg
    push cx
    call write_str
    add sp, 6
    leave
    ret


check_user_program:
    enter 0, 0
    push ds
    mov ax, 07c0h
    mov ds, ax
    mov ax, 050h
    mov es, ax
    mov si, [bp+4]
    mov di, cmd_to_execute
    mov cx, 13
check_user_program_loop:
    cld
    cmpsb
    jnz check_user_program_fail
    or byte [si-1], 0  ; end of string
    loopne check_user_program_loop
    mov si, [bp+4]
    mov ax, [si+0eh]
    pop ds
    leave
    ret
check_user_program_fail:
    mov ax, 0h
    pop ds
    leave
    ret


get_cmd_in_buf:
    enter 0, 0
    push es
    mov ax, ds
    mov es, ax

    cld
    mov si, [bp+4]  ; starting address
    mov di, cmd_to_execute
get_cmd_loop:
    cmp [si], byte ','
    je get_cmd_finish
    or byte [si], 0
    jz get_cmd_finish_all
    movsb
    jmp get_cmd_loop
get_cmd_finish:
    inc si
    mov ax, si
    jmp get_cmd_exit
get_cmd_finish_all:
    mov ax, 0
    jmp get_cmd_exit
get_cmd_exit:
    mov byte [di], 0
    pop es
    leave
    ret


welcome_msg:
    db "Welcome to the OS Operation System!", 0ah, 0dh,\
    "Copyright ", 0a9h, " Richard Tsai (github.com/richardtsai)", 0ah, 0dh, 0ah, 0dh,\
    "Enter '?' to see help", 0ah, 0dh
welcome_msg_len equ ($-welcome_msg)

console_prompt:
    db '#> ', 0
console_prompt_len equ 3

cmd_not_found_msg:
    db 'Command not found'
cmd_not_found_msg_len equ ($-cmd_not_found_msg)

cmd_buf:
    resb 128
cmd_to_execute:
    resb cmd_to_execute_len
cmd_to_execute_len equ 16
