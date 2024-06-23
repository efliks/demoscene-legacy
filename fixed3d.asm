; (C) May 25, 2001  M. Feliks

include sys.inc
include flat3.inc

MAX_POINTS equ 8
MAX_FACES equ 12

.model tiny
.code
.386

org 100h


entrypoint:

    call    do_startup
    call    init_sin_cos

main_loop:
    mov     si, offset object
    mov     di, offset object_rotated
    mov     cx, MAX_POINTS*3
    cld
    rep     movsw

    mov     si, offset angle_x
    mov     di, offset object_rotated
    mov     cx, MAX_POINTS
    call    rotate_points

    mov     si, offset object_rotated
    mov     di, offset object_translated
    mov     cx, MAX_POINTS
    call    translate_points

    call    draw_faces

    call    timer_wait
    call    copy_buffer
    call    clear_buffer

    add     angle_x, 2
    add     angle_y, 3
    add     angle_z, 1

    in      al, 60h
    cmp     al, 1
    jne     main_loop

    call    do_shutdown

draw_faces proc

@@x1 equ word ptr [bp-2]
@@y1 equ word ptr [bp-4]
@@x2 equ word ptr [bp-6]
@@y2 equ word ptr [bp-8]
@@x3 equ word ptr [bp-10]
@@y3 equ word ptr [bp-12]

    push    bp
    mov     bp, sp
    sub     sp, 12

    mov     si, offset link
    mov     cx, MAX_FACES
df_draw:
    push    cx
    mov     cx, 3
    mov     di, bp
    sub     di, 2
df_get_point:
    xor     bh, bh
    mov     bl, byte ptr [si]
    shl     bx, 2
    mov     ax, word ptr object_translated[bx]
    mov     word ptr ss:[di], ax
    mov     ax, word ptr object_translated[bx+2]
    mov     word ptr ss:[di-2], ax
    inc     si
    sub     di, 4
    dec     cx
    jnz     df_get_point

    mov     ax, @@y1
    sub     ax, @@y3
    mov     bx, @@x2
    sub     bx, @@x1
    imul    bx
    shl     edx, 16
    mov     dx, ax
    push    edx
    mov     ax, @@x1
    sub     ax, @@x3
    mov     bx, @@y2
    sub     bx, @@y1
    imul    bx
    shl     edx, 16
    mov     dx, ax
    pop     ebx
    sub     ebx, edx
    or      ebx, ebx
    jge     df_next

    xor     ah, ah
    mov     al, byte ptr [si]

    push    ax
    push    @@y3
    push    @@x3
    push    @@y2
    push    @@x2
    push    @@y1
    push    @@x1
    call    flat_triangle
df_next:
    inc     si
    pop     cx
    dec     cx
    jnz     df_draw

    mov     sp, bp
    pop     bp
    ret
endp

;------------------------------------------------------------
;    ds:si    - offset to angles
;    ds:di    - offset to 3d points
;    cx    - number of points
;------------------------------------------------------------

rotate_points proc

@@sin_x equ word ptr [bp-2]
@@cos_x equ word ptr [bp-4]
@@sin_y equ word ptr [bp-6]
@@cos_y equ word ptr [bp-8]
@@sin_z equ word ptr [bp-10]
@@cos_z equ word ptr [bp-12]

@@x equ word ptr [di]
@@y equ word ptr [di+2]
@@z equ word ptr [di+4]

    push    bp
    mov     bp, sp
    sub     sp, 12

    push    di
    mov     di, bp
    sub     di, 2
    mov     dx, 3
rp_sin_cos:
    mov     bx, word ptr [si]
    and     bx, 511
    shl     bx, 1
    mov     ax, word ptr sin_table[bx]
    mov     word ptr [di], ax
    mov     ax, word ptr cos_table[bx]
    mov     word ptr [di-2], ax

    add     si, 2
    sub     di, 4
    dec     dx
    jnz     rp_sin_cos
    pop     di

rp_rotate:

      ; rotate around x-axis

    mov     ax, @@y
    imul    @@cos_x
    mov     bx, ax
    mov     si, dx

    mov     ax, @@z
    imul    @@sin_x
    sub     bx, ax
    sbb si, dx
    shrd bx, si, 14
    push    bx

    mov     ax, @@y
    imul    @@sin_x
    mov     bx, ax
    mov     si, dx

    mov     ax, @@z
    imul    @@cos_x
    add     bx, ax
    adc si, dx
    shrd bx, si, 14

    pop     @@y
    mov     @@z, bx

      ; rotate around y-axis

    mov     ax, @@x
    imul    @@cos_y
    mov     bx, ax
    mov     si, dx

    mov     ax, @@z
    imul    @@sin_y
    sub     bx, ax
    sbb si, dx
    shrd bx, si, 14
    push    bx

    mov     ax, @@x
    imul    @@sin_y
    mov     bx, ax
    mov     si, dx

    mov     ax, @@z
    imul    @@cos_y
    add     bx, ax
    adc si, dx
    shrd bx, si, 14

    pop     @@x
    mov     @@z, bx

      ; rotate around z-axis

    mov     ax, @@x
    imul    @@cos_z
    mov     bx, ax
    mov     si, dx

    mov     ax, @@y
    imul    @@sin_z
    sub     bx, ax
    sbb si, dx
    shrd bx, si, 14
    push    bx

    mov     ax, @@x
    imul    @@sin_z
    mov     bx, ax
    mov     si, dx

    mov     ax, @@y
    imul    @@cos_z
    add     bx, ax
    adc si, dx
    shrd bx, si, 14

    pop     @@x
    mov     @@y, bx

    add     di, 6
    dec     cx
    jnz     rp_rotate

    mov     sp, bp
    pop     bp
    ret
endp

;------------------------------------------------------------
;    ds:si    - offset to 3d points
;    es:di    - offset to 2d points
;    cx    - number of points
;------------------------------------------------------------

translate_points proc

    mov     bx, word ptr [si+4]
    add     bx, 256

    mov     ax, word ptr [si]
    shl     ax, 8
    cwd
    idiv    bx
    add     ax, 160
    stosw

    mov     ax, word ptr [si+2]
    shl     ax, 8
    cwd
    idiv    bx
    add     ax, 100
    stosw

    add     si, 6
    dec     cx
    jnz     translate_points
    ret
endp

init_sin_cos proc
    push    bp
    mov     bp, sp
    sub     sp, 4

    fldz
    fstp    dword ptr [bp-4]
    xor     di, di
    mov     cx, 512
isc_make:
    fld     dword ptr [bp-4]
    fld     st(0)
    fld     st(0)
    fsin
    fmul    fixed_point_const
    fistp   word ptr sin_table[di]
    fcos
    fmul    fixed_point_const
    fistp   word ptr cos_table[di]

    fadd    inc_angle
    fstp    dword ptr [bp-4]

    add     di, 2
    loop    isc_make

    mov     sp, bp
    pop     bp
    ret
endp

.data

angle_x dw 0
angle_y dw 0
angle_z dw 0

fixed_point_const dd 16384.0
inc_angle dd 0.01227184630309  ; pi/256

object    dw -50, -50, -50,  50, -50, -50,  50, 50, -50,  -50, 50, -50
    dw -50, -50, 50,  50, -50, 50,  50, 50, 50,  -50, 50, 50

link    db 0, 1, 2, 45,  0, 2, 3, 45,  5, 4, 7, 32,  5, 7, 6, 32,  1, 5, 6, 40,  1, 6, 2, 40
    db 4, 0, 3, 80,  4, 3, 7, 80,  4, 5, 1, 60,  1, 0, 4, 60,  3, 2, 6, 94,  3, 6, 7, 94

.data?

sin_table dw 512 dup(?)
cos_table dw 512 dup(?)

object_rotated dw MAX_POINTS dup(?, ?, ?)
object_translated dw MAX_POINTS dup(?, ?)

end entrypoint
