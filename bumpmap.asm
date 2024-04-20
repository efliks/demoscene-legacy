; (C) May 8, 2001  M. Feliks

include sys.inc

BMP_HEADER_SIZE equ 1078

.model tiny
.code
.386

org 100h


entrypoint:
    call    do_startup

    call    alloc_seg
    or      ax, ax
    jz      end_of_prog
    mov     texture_seg, ax
    call    alloc_seg
    or      ax, ax
    jz      end_of_prog
    mov     light_seg, ax

    call    init_light_map
    call    init_way
    call    make_random_bump_map

    push    es
    mov     es, buffer_seg
    xor     di, di
    mov     cx, 80
    xor     eax, eax
    cld
    rep     stosd
    add     di, 64000-640
    mov     cx, 80
    rep     stosd
    pop     es

    call    init_palette

main_loop:
    mov     bx, way_pos_x
    shl     bx, 1
    mov     ax, word ptr way[bx]
    add     ax, 160
    mov     light_x, ax
    mov     bx, way_pos_y
    shl     bx, 1
    mov     ax, word ptr way[bx]
    add     ax, 100
    mov     light_y, ax
    call    do_main_bump

    add     way_pos_x, 2
    and     way_pos_x, 255
    add     way_pos_y, 1
    and     way_pos_y, 255

    call    wait_for_vsync
    call    copy_buffer

    mov     ah, 1h
    int     16h
    jnz     key_hit
    jmp     main_loop
key_hit:
    xor     ah, ah
    int     16h

end_of_prog:
    mov     ax, light_seg
    call    free_seg
    mov     ax, texture_seg
    call    free_seg

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

make_random_bump_map proc
    push    es
    mov     es, texture_seg
    xor     di, di

mb_loop:
    push    word ptr 255
    push    word ptr 0
    call    random
    stosb
    or      di, di
    jnz     mb_loop

    mov     dx, 4
mb_blur_map:
    xor     di, di
    xor     bx, bx
mb_blur:
    mov     al, byte ptr es:[di-1]
    mov     bl, byte ptr es:[di+1]
    add     ax, bx
    mov     bl, byte ptr es:[di-320]
    add     ax, bx
    mov     bl, byte ptr es:[di+320]
    add     ax, bx
    shr     ax, 2
    stosb
    or      di, di
    jnz     mb_blur

    dec     dx
    jnz     mb_blur_map
    pop     es
    ret
endp

do_main_bump proc
    push    es
    push    fs
    push    gs
    mov     es, buffer_seg
    mov     fs, light_seg
    mov     gs, texture_seg

    mov     di, 320
    mov     dx, 1
bump_y:
    xor     cx, cx
bump_x:
    xor     ah, ah
    mov     al, byte ptr gs:[di-1]
    xor     bh, bh
    mov     bl, byte ptr gs:[di+1]
    sub     ax, bx
    push    ax

    xor     ah, ah
    mov     al, byte ptr gs:[di-320]
    xor     bh, bh
    mov     bl, byte ptr gs:[di+320]
    sub     ax, bx
    pop     bx

    mov     si, cx
    sub     si, light_x
    sub     bx, si

    mov     si, dx
    sub     si, light_y
    sub     ax, si

    add     bx, 128
    add     ax, 128

    cmp     bx, 255
    jbe     bump_ok1
    mov     bx, 255
    jmp     bump_ok2
bump_ok1:
    or      bx, bx
    jae     bump_ok2
    xor     bx, bx
    jmp     bump_ok3
bump_ok2:
    cmp     ax, 255
    jbe     bump_ok3
    mov     ax, 255
    jmp     bump_ok4
bump_ok3:
    or      ax, ax
    jae     bump_ok4
    xor     ax, ax
bump_ok4:
    shl     ax, 8
    add     bx, ax
    mov     al, byte ptr fs:[bx]
    stosb

    inc     cx
    cmp     cx, 320
    jne     bump_x

    inc     dx
    cmp     dx, 199
    jne     bump_y

    pop     gs
    pop     fs
    pop     es
    ret
endp

init_palette proc
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
    loop    ip_loop
    ret
endp

COMMENT #
init_texture proc
    push    es
    mov     es, texture_seg
    xor     di, di

    xor     dx, dx
it_ver:
    xor     cx, cx
it_hor:
    mov     ax, cx
    xor     ax, dx
    and     al, 00011111b
    stosb
    inc     cx
    cmp     cx, 320
    jne     it_hor
    inc     dx
    cmp     dx, 200
    jne     it_ver

    pop     es
    ret
endp #

init_light_map proc
    push    es
    mov     es, light_seg
    xor     di, di

    mov     dx, -128
ilm_ver:
    mov     cx, -128
ilm_hor:
    mov     _f16, cx
    fild    _f16
    fdiv    mul_const1
    fmul    st, st
    mov     _f16, dx
    fild    _f16
    fdiv    mul_const1
    fmul    st, st
    faddp   st(1), st
    fsqrt
    fchs
    fld1
    faddp   st(1), st
    mov     _f16, 0
    ficom   _f16
    fstsw   _f16
    mov     ah, byte ptr _f16[1]
    sahf
    jae     ilm_ok
    ffree   st
    fldz
ilm_ok:
    fmul    mul_const2
    fistp   _f16
    mov     ax, _f16
    stosb
    inc     cx
    cmp     cx, 128
    jne     ilm_hor
    inc     dx
    cmp     dx, 128
    jne     ilm_ver

    pop     es
    ret
endp

init_way proc
    mov     di, offset way
    xor     cx, cx
iw_loop:
    mov     _f16, cx
    fldpi
    fimul   _f16
    mov     ax, 128
    mov     _f16, ax
    fidiv   _f16
    fsin
    fimul   _f16
    fistp   _f16
    mov     ax, _f16
    stosw
    inc     cx
    cmp     cx, 320
    jne     iw_loop
    ret
endp

COMMENT #
  ; ds:si - filename
  ; es:di - buffer
  ; cx - width
  ; dx - height
load_bmp proc
    push    dx
    mov     ax, 3d00h
    mov     dx, si
    int     21h
    pop     dx
    jc      lb_file_error

    mov     bx, ax  ; bx - handler

    push    cx
    push    dx
    mov     ax, 4200h
    xor     cx, cx
    mov     dx, BMP_HEADER_SIZE
    int     21h
    pop     dx
    pop     cx

    push    dx
    mov     ax, dx
    dec     ax
    mul     cx
    add     di, ax
    pop     dx

    push    ds
    mov     ax, es
    mov     ds, ax
lb_fill_buffer:
    push    dx
    mov     ax, 3f00h
    mov     dx, di
    int     21h
    sub     di, cx
    pop     dx
    dec     dx
    jnz     lb_fill_buffer
    pop     ds

    mov     ax, 3e00h
    int     21h
    mov     ax, 1
    ret

lb_file_error:
    xor     ax, ax
    ret
endp #

.data

;filename db 'texture.bmp', 0
;error_text db 'Unable to load file texture.bmp', 10, 13, '$'

way_pos_x dw 0
way_pos_y dw 0

random_seed dw 1111h

mul_const1 dd 128.0
mul_const2 dd 255.0

.data?

_f16 dw ?

texture_seg dw ?
light_seg dw ?

light_x dw ?
light_y dw ?
way dw 320 dup(?)

end entrypoint
