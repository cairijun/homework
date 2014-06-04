global farjmp, farcall, memset, get_tr, strlen, strncpy, strncmp, memcpy
farjmp:
    jmp far [esp+4]
    ret

farcall:
    call far [esp+4]
    ret

memset:
    enter 0, 0
    push edi
    mov edi, [ebp+8]
    mov eax, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep stosb
    mov eax, [ebp+8]
    pop edi
    leave
    ret

get_tr:
    str eax
    ret

strlen:
    enter 0, 0
    push edi
    mov edi, [ebp+8]
    mov ecx, -1
    xor al, al
    cld
    repne scasb
    mov eax, -1
    sub eax, ecx
    dec eax
    pop edi
    leave
    ret

strncpy:
    enter 0, 0
    push edi
    push esi
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

    pop esi
    pop edi
    leave
    ret

strncmp:
    enter 0, 0
    push edi
    push esi
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
    pop esi
    pop edi
    leave
    ret

memcpy:
    enter 0, 0
    push edi
    push esi
    mov edi, [ebp+8]
    mov esi, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep movsb
    mov eax, [ebp+8]
    pop esi
    pop edi
    leave
    ret
