; (C) May 1, 2001  M. Feliks

include sys.inc

MAX_COLORS equ 255

.model tiny
.code
.386

org 100h


entrypoint:

    call    do_startup

    mov     si, offset fire_pal
    mov     cx, 768
    xor     ax, ax
    mov     dx, 03c8h
    out     dx, al
    inc     dx
    cld
    rep     outsb

main_loop:
    call    burn
    call    timer_wait
    call    copy_buffer
    
    mov     ah, 6h
    mov     dl, 0ffh
    int     21h
    jz      main_loop

    call    do_shutdown

comment #
make_palette proc
    mov     di, offset fire_pal
    mov     cx, 64
    cld
g_color1:
    xor     al, al
    stosb
    mov     al, ah
    stosb
    xor     al, al
    stosb
    inc     ah
    loop    g_color1

    xor     ax, ax
    mov     cx, 64
g_color2:
    xor     al, al
    stosb
    mov     al, 63
    stosb
    mov     al, ah
    stosb
    inc     ah
    loop    g_color2

    xor     ax, ax
    mov     cx, 64
g_color3:
    mov     al, ah
    stosb
    mov     al, 63
    stosb
    stosb
    inc     ah
    loop    g_color3

    mov     al, 63
    mov     cx, 64
g_color4:
    stosb
    stosb
    stosb
    loop    g_color4
    ret
endp #

burn proc
    push    ds
    mov     ds, buffer_seg

    mov     di, 64000
    mov     cx, 80
burn_loop1:
    mov     bx, cs:[random_seed]
    add     bx, 9248h
    ror     bx, 3
    mov     cs:[random_seed], bx
    mov     ax, MAX_COLORS
    mul     bx
    mov     ax, dx

    mov     ah, al
    mov     dx, ax
    shl     eax, 16
    mov     ax, dx
    mov     dword ptr [di], eax
    mov     dword ptr [di+320], eax
    mov     dword ptr [di+640], eax
    mov     dword ptr [di+960], eax

    add     di, 4
    dec     cx
    jnz     burn_loop1

    mov     di, 640
    mov     cx, 64000+640
    xor     ax, ax
    xor     bx, bx
burn_loop2:
    mov     al, byte ptr [di-1]
    mov     bl, byte ptr [di+1]
    add     ax, bx
    mov     bl, byte ptr [di-320]
    add     ax, bx
    mov     bl, byte ptr [di+320]
    add     ax, bx
    shr     ax, 2
    dec     al
    jz      burn_next
    mov     byte ptr [di-320], al
    mov     byte ptr [di-640], al
burn_next:
    inc     di
    dec     cx
    jnz     burn_loop2

    pop     ds
    ret
endp

.data

random_seed dw 1111h
 
fire_pal db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
 db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
 db    0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   8,   0,   4,   8
 db    0,   4,   8,   0,   4,   8,   0,   4,   8,   0,   4,   8,   0,   4
 db    8,   0,   4,   8,   0,   4,  16,   0,   4,  16,   0,   4,  16,   0
 db    4,  16,   0,   8,  16,   0,   8,  16,   0,   8,  16,   0,   8,  16
 db    8,   8,  16,   8,   8,  16,   8,   8,  16,   8,   8,  16,   8,   8
 db   24,   8,   8,  24,   8,   8,  24,   8,   8,  24,   8,  12,  24,   8
 db   12,  24,   8,  12,  24,   8,  12,  24,   8,  12,  32,   8,  12,  32
 db    8,  12,  32,   8,  12,  32,   8,  12,  32,   8,  12,  32,   8,  12
 db   32,   8,  12,  32,   8,  16,  40,   8,  16,  40,   8,  16,  40,   8
 db   16,  40,   8,  16,  40,   8,  16,  40,   8,  16,  40,   8,  16,  40
 db   16,  16,  40,  16,  16,  40,  16,  16,  40,  16,  16,  40,  16,  20
 db   48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16
 db   20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48
 db   16,  20,  48,  16,  20,  48,  16,  24,  56,  16,  24,  56,  16,  24
 db   56,  16,  24,  56,  16,  24,  56,  16,  24,  56,  16,  24,  56,  16
 db   24,  56,  16,  24,  63,  16,  24,  63,  16,  24,  63,  16,  24,  63
 db   24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28
 db   63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24
 db   28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63
 db   24,  28,  63,  24,  28,  63,  24,  32,  63,  24,  32,  63,  24,  32
 db   63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24
 db   32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63
 db   32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36
 db   63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32
 db   36,  63,  32,  36,  63,  32,  36,  63,  32,  40,  63,  32,  40,  63
 db   32,  40,  63,  32,  40,  63,  32,  40,  63,  32,  40,  63,  32,  40
 db   63,  32,  40,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63,  40
 db   44,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63
 db   48,  48,  63,  48,  48,  63,  48,  48,  63,  48,  48,  63,  48,  52
 db   63,  48,  52,  63,  48,  52,  63,  48,  52,  63,  56,  56,  63,  56
 db   56,  63,  56,  56,  63,  56,  56,  63,  56,  60,  63,  56,  60,  63
 db   56,  60,  63,  56,  60,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db   63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63

end entrypoint
