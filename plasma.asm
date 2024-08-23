; (C) February 20, 2001  M. Feliks

include sys.inc

.model tiny
.code
.386

org 100h


entrypoint:

    call    do_startup
    call    inicjuj_palete
    call    inicjuj_texture

petla:
    call    rotacja_palety
    call    timer_wait

    mov     ah, 6h
    mov     dl, 0ffh
    int     21h
    jz      petla

    call    do_shutdown

rotacja_palety proc
    mov     dx, 03c7h
    xor     ax, ax
    out     dx, al
    add     dx, 2
    mov     di, offset paleta
    mov     cx, 768
    cld
    rep     insb

    mov     si, 3
    mov     bl, paleta[si]
    mov     bh, paleta[si+1]
    mov     dl, paleta[si+2]

    mov     si, offset paleta
    mov     di, si
    add     si, 6
    add     di, 3
    mov     cx, 768-3
    cld
    rep     movsb

    mov     si, 765
    mov     paleta[si], bl
    mov     paleta[si+1], bh
    mov     paleta[si+2], dl

    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     si, offset paleta
    mov     cx, 768
    cld
    rep     outsb
    ret
endp

inicjuj_palete proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 64
kolor1:
    xor     al, al
    out     dx, al
    out     dx, al
    mov     al, ah
    out     dx, al
    inc     ah
    mov     al, ah
    loop    kolor1
    mov     cx, 64
    xor     ax, ax
kolor2:
    xor     al, al
    out     dx, al
    mov     al, ah
    out     dx, al
    mov     al, 63
    out     dx, al
    inc     ah
    loop    kolor2
    mov     cx, 64
    xor     ax, ax
kolor3:
    mov     al, ah
    out     dx, al
    mov     al, 63
    out     dx, al
    out     dx, al
    inc     ah
    loop    kolor3
    mov     cx, 64
    mov     al, 63
kolor4:
    out     dx, al
    out     dx, al
    out     dx, al
    loop    kolor4
    ret
endp

; 127 + 63.5 * sin(x * 3 * (3.14 / 180)) +  63.5 * sin(y * 5 * (3.14 / 180))
inicjuj_texture proc
    fldpi
    mov     _fpom16, 180
    fidiv   _fpom16
    fstp    st_rad

    push    es
    mov     ax, 0a000h
    mov     es, ax
    xor     di, di
    xor     dx, dx
itex_pion:
    xor     cx, cx
    fld     st_rad
    mov     _fpom16, 5
    fimul    _fpom16
    mov     _fpom16, dx
    fimul    _fpom16
    fsin
    fmul    mnoznik
    fstp    _fpom32

itex_poziom:
    fld     st_rad
    mov     _fpom16, 3
    fimul    _fpom16
    mov     _fpom16, cx
    fimul    _fpom16
    fsin
    fmul    mnoznik
    fadd    _fpom32
    mov     _fpom16, 127
    fiadd    _fpom16
    fistp   _fpom16
    mov     ax, _fpom16
    or      al, al
    jnz     itex_dalej
    mov     al, 1

itex_dalej:
    stosb
    inc     cx
    cmp     cx, 320
    jne     itex_poziom
    inc     dx
    cmp     dx, 200
    jne     itex_pion
    pop     es
    ret
endp

.data

mnoznik dd 63.5

.data?

_fpom32 dd ?
_fpom16 dw ?
st_rad dd ?
paleta db 768 dup(?)

end entrypoint
