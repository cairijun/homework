global farjmp, farcall, memset, get_tr, strlen, strncpy, strncmp, memcpy, strchr, toupper, tolower, strrchr
farjmp:
    jmp far [esp+4]
    ret

farcall:
    call far [esp+4]
    ret

memset:
    enter 0, 0
    push edi
    pushf
    mov edi, [ebp+8]
    mov eax, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep stosb
    mov eax, [ebp+8]
    popf
    pop edi
    leave
    ret

get_tr:
    str eax
    ret

strlen:
    enter 0, 0
    push edi
    pushf
    mov edi, [ebp+8]
    mov ecx, -1
    xor al, al
    cld
    repne scasb
    mov eax, -1
    sub eax, ecx
    dec eax
    popf
    pop edi
    leave
    ret

strncpy:
    enter 0, 0
    push edi
    push esi
    pushf
    mov edi, [ebp+8]
    mov esi, [ebp+12]
    mov ecx, [ebp+16]

    cmp byte [esi], 0
    jz _end_cpy
    cld
_cpy:
    movsb
    cmp byte [esi], 0
    loopnz _cpy
_end_cpy:
    mov byte [edi], 0
    mov eax, [ebp+8]

    popf
    pop esi
    pop edi
    leave
    ret

strncmp:
    enter 0, 0
    push edi
    push esi
    pushf
    mov esi, [ebp+8]
    mov edi, [ebp+12]
    mov ecx, [ebp+16]

    cld
_cmp:
    cmp byte [esi], 0
    jz _src_end
    cmpsb
    jg _cmp_g
    jl _cmp_l
    loop _cmp
    jmp _cmp_e

_src_end:
    cmp byte [edi], 0
    jg _cmp_l
    jmp _cmp_e

_cmp_g:
    mov eax, 1
    jmp _cmp_end
_cmp_l:
    mov eax, -1
    jmp _cmp_end
_cmp_e:
    xor eax, eax

_cmp_end:
    popf
    pop esi
    pop edi
    leave
    ret

memcpy:
    enter 0, 0
    push edi
    push esi
    pushf
    mov edi, [ebp+8]
    mov esi, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep movsb
    mov eax, [ebp+8]
    popf
    pop esi
    pop edi
    leave
    ret

strchr:
    enter 0, 0
    push edi
    mov edi, [ebp+8]
    mov al, [ebp+12]
_strchr_loop:
    cmp [edi], al
    je _strchr_found
    cmp byte [edi], 0
    je _strchr_not_found
    inc edi
    jmp _strchr_loop
_strchr_not_found:
    xor eax, eax
    jmp _strchr_end
_strchr_found:
    mov eax, edi
_strchr_end:
    pop edi
    leave
    ret

strrchr:
    enter 0, 0
    push edi
    pushf
    mov edi, [ebp+8]
    push edi
    call strlen
    pop edi
    add edi, eax
    mov ecx, eax
    mov al, [ebp+12]
    std
    repne scasb
    cmp ecx, 0
    jne _strrchr_found
    xor eax, eax
    jmp _strrchr_end
_strrchr_found:
    mov eax, edi
    inc eax
_strrchr_end:
    popf
    pop edi
    leave
    ret

toupper:
    mov eax, [esp+4]
    cmp eax, 'a'
    jl _2up_lo_ori
    cmp eax, 'z'
    jg _2up_lo_ori
    add eax, 'A' - 'a'
    ret
tolower:
    mov eax, [esp+4]
    cmp eax, 'A'
    jl _2up_lo_ori
    cmp eax, 'Z'
    jg _2up_lo_ori
    add eax, 'a' - 'A'
    ret
_2up_lo_ori:
    ret
