; (C) May 7, 2001  M. Feliks

include sys.inc

MAX_STARS equ 300

STAR struc
   x dw ?
   y dw ?
   z dw ?
   c db ?
   clear_me dw ?
ends

.model tiny
.code
.386

org 100h


entrypoint:

    call    do_startup

    mov     si, offset stars_table
    mov     cx, MAX_STARS
is_loop:
    push    word ptr 160
    push    word ptr -160
    call    random
    mov     word ptr [si].x, ax

    push    word ptr -100
    push    word ptr 100
    call    random
    mov     word ptr [si].y, ax

    push    word ptr 100
    push    word ptr -100
    call    random
    mov     word ptr [si].z, ax

    push    word ptr 255
    push    word ptr 0
    call    random
    mov     byte ptr [si].c, al

    add     si, type STAR
    dec     cx
    jnz     is_loop

; initialize palette
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 256
ip_loop:
    out     dx, al
    out     dx, al
    out     dx, al
    inc     ah
    mov     al, ah
    shr     al, 2
    dec     cx
    jnz     ip_loop

    call    clear_buffer

main_loop:
    push    es
    mov     es, buffer_seg

    mov     si, offset stars_table
    mov     cx, MAX_STARS
smp_loop:
    mov     bx, word ptr [si].z
    add     bx, 256

    mov     ax, word ptr [si].x
    shl     ax, 8
    cwd
    idiv    bx
    add     ax, 160
    push    ax

    mov     ax, word ptr [si].y
    shl     ax, 8
    cwd
    idiv    bx
    add     ax, 100
    pop     bx

    or      bx, bx
    jb      smp_reinit
    cmp     bx, 318
    ja      smp_reinit
    or      ax, ax
    jb      smp_reinit
    cmp     ax, 198
    ja      smp_reinit

    mov     di, ax
    shl     di, 6
    shl     ax, 8
    add     ax, di
    add     ax, bx
    mov     di, ax
    mov     word ptr [si.clear_me], ax

    dec     word ptr [si].z

    mov     al, byte ptr [si].c
    mov     ah, al
    stosw
    add     di, 318
    stosw
    cmp     al, 255
    jae     smp_new_color
    inc     al
smp_new_color:
    mov     byte ptr [si].c, al
    jmp     smp_next

smp_reinit:
    push    word ptr 100
    push    word ptr -100
    call    random
    mov     word ptr [si].z, ax
    mov     byte ptr [si].c, 0
smp_next:
    add     si, type STAR
    dec     cx
    jnz     smp_loop
    pop     es

    call    timer_wait
    call    copy_buffer

    push    es
    mov     es, word ptr [buffer_seg]
    mov     si, offset stars_table
    mov     cx, MAX_STARS
    xor     ax, ax
erase_stars:
    mov     di, word ptr [si.clear_me]
    stosw
    add     di, 320 - 2
    stosw
    add     si, type STAR
    loop    erase_stars
    pop     es

    mov     ah, 6h
    mov     dl, 0ffh
    int     21h
    jz      main_loop

    call    do_shutdown

random proc                  ; +4 min,  +6 max
    push    bp
    mov     bp, sp

    mov     bx, random_seed
    add     bx, 9248h
    ror     bx, 3
    mov     random_seed, bx
    mov     ax, word ptr [bp+6]
    sub     ax, word ptr [bp+4]
    mul     bx
    mov     ax, dx
    add     ax, word ptr [bp+4]  ; ax - random number

    pop     bp
    ret     4
endp

.data?

stars_table STAR MAX_STARS dup(?)
random_seed dw ?

end entrypoint
