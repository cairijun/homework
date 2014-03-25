[BITS 16]
global get_kb_char, write_str, write_char, set_char, read_disk, clear_scn
global sleep, move_cursor, get_cursor, get_char

;get_kb_char
;Get keyboard character
;Returns: AL: ASCII character of the key pressed
get_kb_char:
    mov ah, 10h
    int 16h
    pop ecx
    jmp cx

;clear_scn
;Clear screen
clear_scn:
    mov ax, 03h
    int 10h
    pop ecx
    jmp cx

;write_str
;Write a string at a given position
;Parameters:
;   0: Offset of the string to write (DS should be set to the correct segment)
;   1: Length of the string
;   2: Row(high4) and column(low4) of the position
write_str:
    enter 0, 0
    push bx
    mov dx, [bp+14]
    mov cx, [bp+10]

    push bp
    mov bp, [bp+6]

    mov ax, ds
    mov es, ax
    mov ah, 13h
    mov al, 1h
    mov bx, 05h
    int 10h
    pop bp

    pop bx
    leave
    pop ecx
    jmp cx

;write_char
;Write a char at current position with teletype model
;Parameters:
;   0: Char to write
write_char:
    enter 0, 0
    push bx
    mov al, [bp+6]
    mov bx, 0
    mov ah, 0eh
    push bp ; It's said that this bios call may destroy BP
    int 10h
    pop bp
    pop bx
    leave
    pop ecx
    jmp cx

;set_char
;Set a char at a given position
;Parameters:
;   0: Char to set
;   1: Row(high4) and column(low4) of the position
set_char:
    enter 0, 0
    push bx
    
    mov bh, 0
    mov dx, [bp+10]
    mov ah, 02h
    int 10h

    mov al, [bp+6]
    mov bx, 0fh
    mov cx, 1
    mov ah, 09h
    int 10h

    pop bx
    leave
    pop ecx
    jmp cx

;read_disk
;Read some sectors into memory
;Parameters:
;   0: Destination (DS should be set to the correct segment)
;   1: Number of sectors to read (1-255)
;   2: Starting LBA (Can't access cylinder > 255)
;   3: Drive number (0-255, bit 7 set for hard disk)
; ;   2: Cylinder number (0-255 for floppy, 0-1023 for hard disk)
; ;   3: Starting sector number (1-61)
; ;   4: Head number (0-255)
; ;   5: Drive number (0-255)
read_disk:
    enter 0, 0
    push bx
    push si
    push di

    mov ah, 08h
    mov dl, [bp+18]
    int 13h
    inc dh

    mov ax, ds
    mov es, ax

    mov ah, 0h
    mov al, cl
    mul dh

    mov bx, ax
    mov ax, [bp+14]
    ;div bx
    div bl
    mov bl, al ;bl stores the Cylinder number

    mov ax, [bp+14]
    div cl
    mov ah, 0h ;clear the remainder in ah
    div dh
    mov bh, ah ;bh stores the Head number

    mov ax, [bp+14]
    div cl
    inc ah ;ah stores the Sector number
    
    mov al, [bp+10]
    mov ch, bl
    mov cl, ah
    mov dh, bh
    mov dl, [bp+18]
    mov bx, [bp+6]
    mov ah, 02h
    int 13h

    pop di
    pop si
    pop bx
    leave
    pop ecx
    jmp cx

;sleep
;Sleep for some time
;Parameters:
;   0: Interval to sleep in 65536 microseconds
sleep:
    enter 0, 0
    mov dx, 0
    mov cx, [bp+6]
    mov ah, 86h
    int 15h
    leave
    pop ecx
    jmp cx

;move_cursor
;Move the cursor to specified position
;   0: The position to be moved to
move_cursor:
    enter 0, 0
    push bx
    mov dx, [bp+6]
    mov bh, 0h
    mov ah, 2h
    int 10h
    pop bx
    leave
    pop ecx
    jmp cx

;get_cursor
;Get the cursor position
;Returns: AX: The cursor position
get_cursor:
    push bx
    mov bh, 0h
    mov ah, 3h
    int 10h
    mov ax, dx
    pop bx
    pop ecx
    jmp cx

;get_char
;Get the character at current cursor position
;Returns: AL: The character
get_char:
    push bx
    mov bh, 0h
    mov ah, 08h
    int 10h
    pop bx
    pop ecx
    jmp cx
