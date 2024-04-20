; (C) May 3, 2001  M. Feliks

include sys.inc

MAX_BLOBS equ 3
MAX_WAY_POINTS equ 360*2

.model tiny
.code
.386

org 100h


entrypoint:
    call    do_startup

      ; invert palette
    mov     di, offset palette + 3
    mov     cx, 768 - 3
i_pal:
    mov     al, 63
    sub     al, byte ptr [di]
    stosb
    loop    i_pal

    call    init_way
    call    init_blobs

    mov     si, offset palette
    call    set_palette

main_loop:
    call    move_blobs
    call    make_frame
    call    wait_for_vsync
    call    copy_buffer

    mov     ah, 1h
    int     16h
    jnz     key_hit
    jmp     main_loop
key_hit:
    xor     ah, ah
    int     16h

    call    do_shutdown

make_frame proc
    push    es
    push    bp
    mov     bp, sp
    sub     sp, 4
                      ; bp-2 - y counter
                      ; bp-4 - x counter
    mov     es, buffer_seg
    xor     di, di

    mov     word ptr [bp-2], 200
mf_y_loop:
    mov     word ptr [bp-4], 320
mf_x_loop:
    mov     cx, MAX_BLOBS
    mov     si, offset blobs_table
    xor     dx, dx  ; dx - color
mf_blob_loop:
    push    dx

    mov     ax, word ptr [bp-4]  ; sceen_x
    sub     ax, word ptr [si]  ; blob_x
    imul    ax
    shl     edx, 16
    mov     dx, ax
    mov     ebx, edx  ; ebx = (screen_x-blob_x)^2

    mov     ax, word ptr [bp-2]  ; screen_y
    sub     ax, word ptr [si+2]  ; blob_y
    imul    ax
    shl     edx, 16
    mov     dx, ax  ; edx = (screen_y-blob_y)^2

    add     ebx, edx  ; ebx = (screen_x-blob_x)^2 + (screen_y-blob_y)^2
    inc     ebx
    mov     eax, 150000
    xor     edx, edx
    div     ebx  ; 80000/ebx

    pop     dx
    add     dx, ax
    cmp     dx, 255
    jb      mf_ok_col
    mov     dx, 255
mf_ok_col:

    add     si, 4
    dec     cx
    jnz     mf_blob_loop

    mov     dh, dl
    mov     word ptr es:[di], dx
    mov     word ptr es:[di + 320], dx

    add     di, 2

    sub     word ptr [bp-4], 2
    jnz     mf_x_loop

    add     di, 320
    sub     word ptr [bp-2], 2
    jnz     mf_y_loop

    mov     sp, bp
    pop     bp
    pop     es
    ret
endp

comment #
make_frame proc
    push    bp
    push    es
    sub     sp, 2
    mov     bp, sp

    mov     es, buffer_seg
    xor     di, di

    mov     dx, 200
mf_y:
    mov     cx, 320
mf_x:
    mov     bx, MAX_BLOBS
    xor     ax, ax
    mov     si, offset blobs_table
mf_blob:
    mov     word ptr [bp], cx
    fild    word ptr [bp]
    fisub    word ptr [si]
    fmul    st, st

    mov     word ptr [bp], dx
    fild    word ptr [bp]
    fisub    word ptr [si+2]
    fmul    st, st

    faddp   st(1), st
    fdivr   mm_const
    fistp   word ptr [bp]
    add     ax, word ptr [bp]

    add     si, 4
    dec     bx
    jnz     mf_blob

    mov     ah, al
    stosw
    mov     word ptr es:[di+320-2], ax
    sub     cx, 2
      ;dec cx
    jnz     mf_x

    add     di, 320
      ;dec dx
    sub     dx, 2
    jnz     mf_y

    add     sp, 2
    pop     es
    pop     bp
    ret
endp
#

init_palette proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 64
g_color1:
    xor     al, al
    out     dx, al
    mov     al, ah
    out     dx, al
    xor     al, al
    out     dx, al
    inc     ah
    loop    g_color1

    xor     ax, ax
    mov     cx, 64
g_color2:
    xor     al, al
    out     dx, al
    mov     al, 63
    out     dx, al
    mov     al, ah
    out     dx, al
    inc     ah
    loop    g_color2

    xor     ax, ax
    mov     cx, 64
g_color3:
    mov     al, ah
    out     dx, al
    mov     al, 63
    out     dx, al
    out     dx, al
    inc     ah
    loop    g_color3

    mov     al, 63
    mov     cx, 64
g_color4:
    out     dx, al
    out     dx, al
    out     dx, al
    loop    g_color4
    ret
endp

init_blobs proc
    mov     di, offset blob_position
    mov     cx, MAX_BLOBS
      ;mov dx, MAX_WAY_POINTS/MAX_BLOBS
    xor     ax, ax
    cld
ib_loop:
    stosw
      ;add ax, dx
    add     ax, 50
    dec     cx
    jnz     ib_loop
    ret
endp

move_blobs proc
    mov     si, offset blob_position
    mov     di, offset blobs_table
    mov     cx, MAX_BLOBS
mb_loop:
    mov     bx, word ptr [si]
    mov     dx, bx
    shl     bx, 2
    mov     ax, word ptr way[bx]
    stosw
    mov     ax, word ptr way[bx+2]
    stosw

    inc     dx
    cmp     dx, MAX_WAY_POINTS
    jb      mb_ok
    xor     dx, dx
mb_ok:
    mov     word ptr [si], dx
    add     si, 2
    dec     cx
    jnz     mb_loop
    ret
endp

init_way proc
    push    bp
    mov     bp, sp
    sub     sp, 4

    mov     di, offset way
    xor     cx, cx
    mov     word ptr [bp-4], 0
    cld
iw_loop:
    fld     dword ptr [bp-4]
    fld     st(0)
    fsin
    fmul    big_radius
    fxch    st(1)
    fmul    mul_const
    fsin
    fmul    small_radius
    faddp   st(1), st
    fadd    correct_x
    fistp   word ptr [di]

    fld     dword ptr [bp-4]
    fld     st(0)
    fcos
    fmul    big_radius
    fxch    st(1)
    fmul    mul_const
    fcos
    fmul    small_radius
    faddp   st(1), st
    fadd    correct_y
    fistp   word ptr [di+2]

    fld     dword ptr [bp-4]
    fadd    delta_angle
    fstp    dword ptr [bp-4]

    add     di, 4
    inc     cx
    cmp     cx, MAX_WAY_POINTS
    jne     iw_loop

    add     sp, 4
    pop     bp
    ret
endp

.data

mm_const dd 80000.0
mul_const dd 10.0

big_radius dd 60.0
small_radius dd 30.0

correct_x dd 160.0
correct_y dd 100.0

delta_angle dd 0.008726646259972  ; 0.01745329251994/2
   
palette db  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
 db   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
 db   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   8,   0,   4,   8
 db   0,   4,   8,   0,   4,   8,   0,   4,   8,   0,   4,   8,   0,   4
 db   8,   0,   4,   8,   0,   4,  16,   0,   4,  16,   0,   4,  16,   0
 db   4,  16,   0,   8,  16,   0,   8,  16,   0,   8,  16,   0,   8,  16
 db   8,   8,  16,   8,   8,  16,   8,   8,  16,   8,   8,  16,   8,   8
 db  24,   8,   8,  24,   8,   8,  24,   8,   8,  24,   8,  12,  24,   8
 db  12,  24,   8,  12,  24,   8,  12,  24,   8,  12,  32,   8,  12,  32
 db   8,  12,  32,   8,  12,  32,   8,  12,  32,   8,  12,  32,   8,  12
 db  32,   8,  12,  32,   8,  16,  40,   8,  16,  40,   8,  16,  40,   8
 db  16,  40,   8,  16,  40,   8,  16,  40,   8,  16,  40,   8,  16,  40
 db  16,  16,  40,  16,  16,  40,  16,  16,  40,  16,  16,  40,  16,  20
 db  48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16
 db  20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48,  16,  20,  48
 db  16,  20,  48,  16,  20,  48,  16,  24,  56,  16,  24,  56,  16,  24
 db  56,  16,  24,  56,  16,  24,  56,  16,  24,  56,  16,  24,  56,  16
 db  24,  56,  16,  24,  63,  16,  24,  63,  16,  24,  63,  16,  24,  63
 db  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28
 db  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24
 db  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63,  24,  28,  63
 db  24,  28,  63,  24,  28,  63,  24,  32,  63,  24,  32,  63,  24,  32
 db  63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24
 db  32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63,  24,  32,  63
 db  32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36
 db  63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32,  36,  63,  32
 db  36,  63,  32,  36,  63,  32,  36,  63,  32,  40,  63,  32,  40,  63
 db  32,  40,  63,  32,  40,  63,  32,  40,  63,  32,  40,  63,  32,  40
 db  63,  32,  40,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63,  40
 db  44,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63,  40,  44,  63
 db  48,  48,  63,  48,  48,  63,  48,  48,  63,  48,  48,  63,  48,  52
 db  63,  48,  52,  63,  48,  52,  63,  48,  52,  63,  56,  56,  63,  56
 db  56,  63,  56,  56,  63,  56,  56,  63,  56,  60,  63,  56,  60,  63
 db  56,  60,  63,  56,  60,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63
 db  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63,  63

.data?

way dw MAX_WAY_POINTS dup(?, ?)
blobs_table dw MAX_BLOBS dup(?, ?)
blob_position dw MAX_BLOBS dup(?)

random_seed dw ?

end entrypoint
