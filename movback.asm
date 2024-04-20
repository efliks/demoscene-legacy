; (C) July 31, 2001  M. Feliks

include sys.inc

VC_DELTA equ 1
HC_DELTA equ 2

.model tiny
.code
.386
org 100h


entrypoint:
    call    do_startup

    call    alloc_seg
    or      ax, ax
    jz      quit_effect
    mov     background_seg, ax

    call    init_sine
    call    init_background
    call    set_moveback_palette
main_loop:

    add     ver_counter, VC_DELTA
    add     hor_counter, HC_DELTA

    call    animation
    call    wait_for_vsync
    call    copy_buffer

    mov     ah, 1h
    int     16h
    jnz     key_hit
    jmp     main_loop
key_hit:
    xor     ah, ah
    int     16h

quit_effect:
    mov     ax, background_seg
    call    free_seg

    call    do_shutdown


animation proc

@@offx equ word ptr [bp-2]
@@offy equ word ptr [bp-4]

    push    bp
    mov     bp, sp
    sub     sp, 4

    mov     bx, ver_counter
    and     bx, 255
    shl     bx, 1
    mov     ax, word ptr sinetable[bx]
    mov     @@offy, ax
    mov     bx, hor_counter
    and     bx, 255
    shl     bx, 1
    mov     ax, word ptr sinetable[bx]
    mov     @@offx, ax

    push    ds
    push    es
    mov     ax, buffer_seg
    mov     es, ax
    xor     di, di
    mov     ax, background_seg
    mov     ds, ax

    mov     dx, 199
a_ver:
    mov     cx, 319
    mov     bx, @@offy
    add     bx, dx
    and     bx, 255
    shl     bx, 8
a_hor:
    mov     ax, @@offx
    add     ax, cx
    and     ax, 255
    mov     si, bx
    add     si, ax
    mov     al, byte ptr [si]
    stosb
    dec     cx
    jge     a_hor
    dec     dx
    jge     a_ver

    pop     es
    pop     ds

    mov     sp, bp
    pop     bp
    ret
endp

set_moveback_palette proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 256
spal_loop:
    mov     al, ah
    shr     al, 2
    out     dx, al
    out     dx, al
    out     dx, al
    inc     ah
    dec     cx
    jnz     spal_loop
    ret
endp

init_background proc
    push    es
    mov     ax, background_seg
    mov     es, ax
    xor     di, di

    xor     dx, dx
ib_ver:
    xor     cx, cx
ib_hor:
    mov     ax, cx
    xor     ax, dx
    stosb
    inc     cx
    cmp     cx, 256
    jne     ib_hor
    inc     dx
    cmp     dx, 256
    jne     ib_ver

    pop     es
    ret
endp

init_sine proc

@@angle equ dword ptr [bp-4]

    push    bp
    mov     bp, sp
    sub     sp, 4

    mov     @@angle, 0
    mov     cx, 256
    mov     di, offset sinetable
is_loop:
    fld     @@angle
    fld     st(0)
    fsin
    fmul    scale_sin
    fistp   word ptr [di]
    fadd    delta_angle
    fstp    @@angle
    add     di, 2
    dec     cx
    jnz     is_loop

    mov     sp, bp
    pop     bp
    ret
endp

.data

delta_angle dd 0.0245436926066  ; pi/128
scale_sin dd 128.0

ver_counter dw 0
hor_counter dw 0

.data?

background_seg dw ?
sinetable dw 256 dup(?)

end entrypoint
