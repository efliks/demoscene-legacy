; Originally written 9/12/2000

include globals.inc
include graphlib.inc

_DATA   segment
alloc_err_str   db  'Memory error!', 10, 13, '$'
buffer_seg      dw  ?

_DATA   ends


_TEXT   segment
.386

copy_screen proc
    push    ds
    push    es
    mov     ax, buffer_seg
    mov     ds, ax
    xor     si, si
    mov     ax, 0a000h
    mov     es, ax
    xor     di, di
    mov     cx, 16000
    cld
    rep     movsd
    pop     es
    pop     ds
    ret
endp


copy_screen_fpu proc
    push    ds
    push    es

    mov     ds, buffer_seg
    xor     si, si
    mov     ax, 0a000h
    mov     es, ax
    xor     di, di
    mov     cx, 64000/8

cb_copy:    
    fld     qword ptr [si]
    add     si, 8
    fstp    qword ptr es:[di]
    add     di, 8
    dec     cx
    jnz     cb_copy

    pop     ds
    pop     es
endp


clear_screen proc
    push    es
    mov     ax, buffer_seg
    mov     es, ax
    xor     di, di
    xor     eax, eax
    mov     cx, 16000
    cld
    rep     stosd
    pop     es
    ret
endp


alloc_screen proc
    mov     ah, 4ah
    mov     bx, 1000h
    int     21h
    mov     ah, 48h
    mov     bx, 1000h
    int     21h
    jc      alloc_error
    mov     buffer_seg, ax
    ret

alloc_error:
    mov     ah, 09h
    mov     dx, offset alloc_err_str
    int     21h
    mov     ah, 4ch
    int     21h
    ret
endp


free_screen proc
    push    es
    mov     ax, buffer_seg
    mov     es, ax
    mov     ah, 49h
    int     21h
    pop     es
    ret
endp


wait_for_frame proc
    mov    dx, 03dah
c1:
    in     al, dx
    test   al, 8
    jz     c1
c2:
    in     al, dx
    test   al, 8
    jnz    c2
    ret
endp


; ds:si -> input address to read palette
set_palette proc
    mov    dx, 03c8h
    xor    ax, ax
    out    dx, al
    inc    dx
    mov    cx, 768
    cld
    rep    outsb
    ret
endp


; ds:di -> output address to write palette
get_palette proc
    mov    dx, 03c7h
    xor    ax, ax
    out    dx, al
    add    dx, 2
    mov    cx, 768
    cld
    rep    insb
    ret
endp

_TEXT    ends

END

