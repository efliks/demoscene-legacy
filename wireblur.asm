; (C) May 5, 2001  M. Feliks

include sys.inc
include math3d.inc

MAX_POINTS equ 44
MAX_LINKS equ 66

.model tiny
.code
.386

org 100h


entrypoint:
    call    do_startup
    call    init_palette

main_loop:
    mov     si, offset object
    mov     di, offset object_rotated
    mov     cx, MAX_POINTS*3
copy_object:
    fild    word ptr [si]
    fstp    dword ptr [di]
    add     si, 2
    add     di, 4
    dec     cx
    jnz     copy_object

    mov     si, offset angle_x
    mov     di, offset object_rotated
    mov     cx, MAX_POINTS
    call    rotate_points_old

    mov     si, offset object_rotated
    mov     di, offset coord2d
    mov     cx, MAX_POINTS
    call    translate_points

    call    draw_wireframe

    call    blur_screen
    call    wait_for_vsync
    call    copy_buffer

    fld     angle_x
    fadd    inc_angle
    fstp    angle_x
    fld     angle_y
    fadd    inc_angle
    fadd    inc_angle
    fstp    angle_y
    fld     angle_z
    fsub    inc_angle
    fstp    angle_z

    mov     ax, oc_delta
    add     object_color, ax

    inc     oc_counter
    cmp     oc_counter, 96
    jb      oc_next
    mov     oc_counter, 0
    neg     oc_delta
oc_next:

    in      al, 60h
    cmp     al, 1
    jne     main_loop

    call    do_shutdown


blur_screen proc
    push    ds
    mov     ds, buffer_seg
    xor     di, di

    xor     eax, eax
    mov     cx, 320/4
bs_clear1:
    mov     dword ptr [di], eax
    add     di, 4
    dec     cx
    jnz     bs_clear1

    xor     bx, bx
    mov     cx, 64000-640
bs_loop:
    mov     al, byte ptr [di-1]
    mov     bl, byte ptr [di-320]
    add     ax, bx
    mov     bl, byte ptr [di+1]
    add     ax, bx
    mov     bl, byte ptr [di+320]
    add     ax, bx
    shr     ax, 2
    jz      bs_next
    dec     al
    mov     byte ptr [di], al
bs_next:
    inc     di
    dec     cx
    jnz     bs_loop

    xor     eax, eax
    mov     cx, 320/4
bs_clear2:
    mov     dword ptr [di], eax
    add     di, 4
    dec     cx
    jnz     bs_clear2

    pop     ds
    ret
endp

init_palette proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 64
ip_color1:
    xor     al, al
    out     dx, al
    out     dx, al
    mov     al, ah
    out     dx, al
    inc     ah
    mov     al, ah
    loop    ip_color1

    mov     cx, 64
    xor     ax, ax
ip_color2:
    xor     al, al
    out     dx, al
    mov     al, ah
    out     dx, al
    mov     al, 63
    out     dx, al
    inc     ah
    loop    ip_color2

    mov     cx, 64
    xor     ax, ax
ip_color3:
    mov     al, ah
    out     dx, al
    mov     al, 63
    out     dx, al
    out     dx, al
    inc     ah
    loop    ip_color3

    mov     cx, 64
    mov     al, 63
ip_color4:
    out     dx, al
    out     dx, al
    out     dx, al
    loop    ip_color4
    ret
endp

draw_wireframe proc
    mov     si, offset link
    mov     cx, MAX_LINKS
dw_loop:
    push    cx

    push    word ptr object_color  ; color

    mov     bx, word ptr [si+2]
    shl     bx, 2
    push    word ptr coord2d[bx+2]  ; y2
    push    word ptr coord2d[bx]  ; x2

    mov     bx, word ptr [si]
    shl     bx, 2
    push    word ptr coord2d[bx+2]  ; y1
    push    word ptr coord2d[bx]  ; x1

    call    draw_bresenham_line

    add     si, 4
    pop     cx
    dec     cx
    jnz     dw_loop
    ret
endp
  
  ; +4  x1
  ; +6  y1
  ; +8  x2
  ; +10 y2
  ; +12 color
draw_bresenham_line proc
    push    bp
    mov     bp, sp
    sub     sp, 12

    ; bp-2 inc1_d
    ; bp-4 inc2_d
    ; bp-6 inc1_x
    ; bp-8 inc2_x
    ; bp-10 inc1_y
    ; bp-12 inc2_y
    
    push    ds
    mov     ds, buffer_seg

    mov     ax, word ptr [bp+8]  ; x2
    sub     ax, word ptr [bp+4]  ; x1
    cwd
    xor     ax, dx
    sub     ax, dx
    mov     bx, ax  ; bx = delta_x

    mov     ax, word ptr [bp+10]  ; y2
    sub     ax, word ptr [bp+6]  ; y1
    cwd
    xor     ax, dx
    sub     ax, dx  ; ax = delta_y

    cmp     bx, ax  ; cmp delta_x, delta_y
    jb      dl_bigger_delta_y

    mov     dx, ax
    shl     dx, 1
    mov     word ptr [bp-2], dx  ; inc1_d = delta_y*2

    sub     dx, bx  ; dx = delta = (delta_y*2)-delta_x

    mov     cx, ax
    sub     cx, bx
    shl     cx, 1
    mov     word ptr [bp-4], cx  ; inc2_d = (delta_y-delta_x)*2

    mov     cx, bx
    inc     cx  ; lenght of line

    mov     word ptr [bp-6], 1  ; inc1_x
    mov     word ptr [bp-8], 1  ; inc2_x
    mov     word ptr [bp-10], 0  ; inc1_y
    mov     word ptr [bp-12], 1  ; inc2_y

    jmp     dl_check_xy

dl_bigger_delta_y:

    mov     dx, bx
    shl     dx, 1
    mov     word ptr [bp-2], dx  ; inc1_d = delta_x*2

    sub     dx, ax  ; dx = delta = (delta_x*2)-delta_y

    mov     cx, bx
    sub     cx, ax
    shl     cx, 1
    mov     word ptr [bp-4], cx  ; inc2_d = (delta_x-delta_y)*2

    mov     cx, ax
    inc     cx

    mov     word ptr [bp-6], 0  ; inc1_x
    mov     word ptr [bp-8], 1  ; inc2_x
    mov     word ptr [bp-10], 1  ; inc1_y
    mov     word ptr [bp-12], 1  ; inc2_y

dl_check_xy:

    mov     ax, word ptr [bp+4]  ; x1
    cmp     ax, word ptr [bp+8]  ; x2
    jl      dl_ok_x  ; x1 < x2

    neg     word ptr [bp-6]  ; inc1_x
    neg     word ptr [bp-8]  ; inc2_x
dl_ok_x:
    mov     ax, word ptr [bp+6]  ; y1
    cmp     ax, word ptr [bp+10]  ; y2
    jl      dl_ok_y  ; y1 < y2

    neg     word ptr [bp-10]  ; inc1_y
    neg     word ptr [bp-12]  ; inc2_y
dl_ok_y:

    mov     ax, word ptr [bp+4]  ; x
    mov     bx, word ptr [bp+6]  ; y
dl_loop:
    or      ax, ax
    jl      dl_next_pixel
    cmp     ax, 319
    jg      dl_next_pixel
    or      bx, bx
    jl      dl_next_pixel
    cmp     bx, 199
    jg      dl_next_pixel

    push    bx
    mov     di, bx
    shl     di, 6
    shl     bx, 8
    add     di, bx
    add     di, ax
    mov     bx, word ptr [bp+12]  ; color
    mov     byte ptr [di], bl  ; set pixel
    pop     bx

dl_next_pixel:
    or      dx, dx
    jge     dl_jump1  ; delta >= 0

    add     dx, word ptr [bp-2]  ; delta+=inc1_d
    add     ax, word ptr [bp-6]  ; x+=inc1_x
    add     bx, word ptr [bp-10]  ; y+=inc1_y
    jmp     dl_jump2
dl_jump1:
    add     dx, word ptr [bp-4]  ; delta+=inc2_d
    add     ax, word ptr [bp-8]  ; x+=inc2_x
    add     bx, word ptr [bp-12]  ; y+=inc2_y
dl_jump2:
    dec     cx
    jnz     dl_loop

    pop     ds
    add     sp, 12
    pop     bp
    ret     10
endp

.data

link    dw 0, 12,  1, 13,  2, 14,  3, 15,  4, 16,  5, 17,  6, 18,  7, 19,  8, 20,  9, 21
    dw 10, 22,  11, 23
    dw 12, 13,  13, 14,  14, 15,  15, 16,  16, 17,  17, 18,  18, 19,  19, 20,  20, 21
    dw 21, 22,  22, 23,  23, 12
    dw 0, 1,  1, 2,  2, 3,  3, 4,  4, 5,  5, 6,  6, 7,  7, 8,  8, 9,  9, 10,  10, 11,  11, 0

    dw 24, 25,  25, 26,  26, 27,  27, 28,  28, 29,  29, 30,  30, 31,  31, 32,  32, 33,  24, 33
    dw 34, 35,  35, 36,  36, 37,  37, 38,  38, 39,  39, 40,  40, 41,  41, 42,  42, 43,  34, 43
    dw 24, 34,  25, 35,  26, 36,  27, 37,  28, 38,  29, 39,  30, 40,  31, 41,  32, 42,  33, 43

object    dw -115, 50, 25,  -115, -50, 25,  -90, -50, 25
    dw -65, -25, 25,  -40, -50, 25,  -15, -50, 25
    dw -15, 50, 25,  -40, 50, 25,  -40, 0, 25
    dw -65, 25, 25,  -90, 0, 25,  -90, 50, 25

    dw -115, 50, -25,  -115, -50, -25,  -90, -50, -25
    dw -65, -25, -25,  -40, -50, -25,  -15, -50, -25
    dw -15, 50, -25,  -40, 50, -25,  -40, 0, -25
    dw -65, 25, -25,  -90, 0, -25,  -90, 50, -25

    dw 15, 50, 25,  15, -50, 25,  115, -50, 25
    dw 115, -25, 25,  65, -25, 25,  65, -15, 25
    dw 100, -15, 25,  100, 15, 25,  65, 15, 25,  65, 50, 25

    dw 15, 50, -25,  15, -50, -25,  115, -50, -25
    dw 115, -25, -25,  65, -25, -25,  65, -15, -25
    dw 100, -15, -25,  100, 15, -25,  65, 15, -25,  65, 50, -25

inc_angle dd 0.0174533
angle_x dd 0.0
angle_y dd 0.0
angle_z dd 0.0

object_color dw 64
oc_delta dw 1
oc_counter dw 0

.data?

object_rotated dd MAX_POINTS dup(?, ?, ?)
coord2d dw MAX_POINTS dup(?, ?)

end entrypoint
