[ORG 0100h]
_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, msg_len
    int 80h

    mov eax, 3
    mov ebx, 0
    mov ecx, buf
    mov edx, 64
    int 80h

    mov eax, 4
    mov ebx, 1
    mov ecx, feedback
    mov edx, feedback_len
    int 80h

    mov edx, eax
    mov eax, 4
    mov ebx, 1
    mov ecx, buf
    int 80h

    mov eax, 1
    int 80h

msg:
    db "This program use the read and write system call to", 0xa,\
       "read and write from the terminal. When you press Ctrl-D", 0xa,\
       "the reading will stop. After that, the program will exit", 0xa,\
       "via the _exit system call.", 0xa,\
       "Please enter some things: (your input will NOT echo)", 0xa
msg_len equ ($-msg)

buf:
    resb 64

feedback:
    db 0xa, "Your input: ", 0xa
feedback_len equ($-feedback)
