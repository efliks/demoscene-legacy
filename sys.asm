include sys.inc

.model tiny
.code
.386

org 100h


do_startup proc
    finit
    
    mov     ax, cs
    mov     ds, ax
    mov     es, ax

    call    alloc_seg
    or      ax, ax
    jz      quit_me
    mov     buffer_seg, ax

    mov     ax, 13h
    int     10h
    ret
quit_me:
    mov     ah, 4ch
    int     21h
endp

do_shutdown proc
    mov     ax, 3h
    int     10h
    mov     ax, buffer_seg
    call    free_seg

    mov     ah, 4ch
    int     21h
endp

copy_buffer proc

comment #
      ; copy via coprocessor
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
#

    push    ds
    push    es
    mov     ds, buffer_seg
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

clear_buffer proc
    push    es
    mov     es, buffer_seg
    xor     di, di
    xor     eax, eax
    mov     cx, 16000
    cld
    rep     stosd
    pop     es
    ret
endp

wait_for_vsync proc
    mov     dx, 03dah
r1:
    in      al, dx
    test    al, 8
    jz      r1
r2:
    in      al, dx
    test    al, 8
    jnz     r2
    ret
endp

;------------------------------------------------------------
;    in:    si - offset to palette
;    out:    none
;------------------------------------------------------------

set_palette proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 768
    cld
    rep     outsb
    ret
endp

;------------------------------------------------------------
;    in:    di - offset to palette
;    out:    none
;------------------------------------------------------------

get_palette proc
    mov     dx, 03c7h
    xor     ax, ax
    out     dx, al
    add     dx, 2
    mov     cx, 768
    cld
    rep     insb
    ret
endp

;------------------------------------------------------------
;    in:    none
;    out:    ax - segment number (0 if error occured)
;------------------------------------------------------------

alloc_seg proc
    mov     ah, 4ah
    mov     bx, 1000h
    int     21h
    mov     ah, 48h
    mov     bx, 1000h
    int     21h
    jc      as_no_mem
    ret
as_no_mem:
    xor     ax, ax
    ret
endp

;------------------------------------------------------------
;    in:    ax - segment number
;    out:    none
;------------------------------------------------------------

free_seg proc
    push    es
    mov     es, ax
    mov     ah, 49h
    int     21h
    pop     es
    ret
endp

init_font proc
    push    bp
    push    ds
    push    es

    mov     ax, 01130h
    mov     bh, 03h
    int     10h

    mov     ax, es
    mov     ds, ax
    mov     si, bp

    mov     ax, cs
    mov     es, ax
    mov     di, offset font_data

    mov     cx, 2048/4
    cld
    rep     movsd

    pop     es
    pop     ds
    pop     bp
    ret
endp

;------------------------------------------------------------
;    in:    si - offset to null-terminated string
;        al - color
;        cx - x
;        dx - y
;    out:    none
;------------------------------------------------------------

put_string proc
    push    es
    mov     es, buffer_seg

    mov     di, dx
    shl     di, 6
    shl     dx, 8
    add     di, dx
    add     di, cx

    mov     ah, al
ps_char:
    lodsb
    or      al, al
    jz      ps_quit

      ; draw letter
    push    di
    mov     dl, ah
    movzx   bx, al
    shl     bx, 3
    add     bx, offset font_data

    mov     ch, 8
ps_hor:
    mov     cl, 8
    mov     al, byte ptr [bx]
ps_ver:
    rcl     al, 1
    jae     ps_next
    mov     byte ptr es:[di], dl
ps_next:
    inc     di
    dec     cl
    jnz     ps_ver
    inc     bx
    add     di, 312
    inc     dl
    dec     ch
    jnz     ps_hor
    pop     di

      ; next letter
    add     di, 8
    jmp     ps_char
ps_quit:
    pop     es
    ret
endp

.data?

font_data db 2048 dup(?)
buffer_seg dw ?

end
