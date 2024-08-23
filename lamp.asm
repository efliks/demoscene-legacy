; (C) December 26, 2000  M. Feliks

include sys.inc

MAPA_BOK equ 180

.model tiny
.code
.386

org 100h


entrypoint:

    call    do_startup

    call    alloc_seg
    cmp     ax, 0
    je      koniec
    mov     obrazek_segment, ax

    call    inicjuj_palete
    call    generuj_obrazek
    call    generuj_mape
    call    inicjuj_droge

main_loop:
    mov     ax, obrazek_segment
    mov     bx, buffer_seg
    
    push    ds
    push    es
    mov     ds, ax
    xor     si, si
    mov     es, bx
    xor     di, di
    mov     cx, 16000
    cld
    rep     movsd
    pop     es
    pop     ds

    mov     bx, kat_x
    shl     bx, 1
    mov     ax, ds:droga[bx]
    add     ax, 160
    mov     bx, kat_y
    shl     bx, 1
    mov     dx, ds:droga[bx]
    add     dx, 100
    mov     bx, dx
    call    lampa2

    call    timer_wait
    call    copy_buffer
    call    obrazek_scroll

    mov     ax, word ptr kat_x
    add     ax, 2
    and     ax, 255
    mov     word ptr kat_x, ax

    mov     ax, word ptr kat_y
    add     ax, 1
    and     ax, 255
    mov     word ptr kat_y, ax

    mov     ah, 6h
    mov     dl, 0ffh
    int     21h
    jz      main_loop

koniec:
    mov     ax, obrazek_segment
    call    free_seg

    call    do_shutdown


obrazek_scroll proc
    push    ds
    mov     ax, obrazek_segment
    mov     ds, ax
    xor     si, si
    mov     di, offset linia
    mov     cx, 320/4
    cld
    rep     movsd

    mov     di, 320
    mov     cx, (64000-320)/4
scroll_skok:
    mov     eax, dword ptr [di]
    mov     dword ptr [di-320], eax
    add     di, 4
    dec     cx
    jnz     scroll_skok
    pop     ds

    push    es
    mov     es, obrazek_segment
    mov     si, offset linia
    mov     di, 64000-320
    mov     cx, 320/4
    cld
    rep     movsd
    pop     es
    ret
endp

lampa2 proc
    push    es
    mov     dx, buffer_seg
    mov     es, dx
    mov     si, offset mapa
    mov     bp, sp
    sub     sp, 8

    mov     dx, MAPA_BOK/2
    mov     word ptr [bp-2], ax
    sub     word ptr [bp-2], dx  ;[bp-2] x_start
    mov     word ptr [bp-4], ax
    add     word ptr [bp-4], dx  ;[bp-4] x_koniec

    mov     word ptr [bp-6], bx
    sub     word ptr [bp-6], dx  ;[bp-6] y_start
    mov     word ptr [bp-8], bx
    add     word ptr [bp-8], dx  ;[bp-8] y_koniec

    mov     cx, word ptr [bp-6]
lampa_pion:
    cmp     cx, 0
    jae     lampa_dalej1
    jmp     zmiana_linii
lampa_dalej1:
    cmp     cx, 199
    jbe     lampa_dalej2
zmiana_linii:
    add     si, MAPA_BOK
    jmp     lampa_dalej4
lampa_dalej2:

    mov     bx, cx
    mov     ax, bx
    shl     bx, 6
    shl     ax, 8
    add     bx, ax  ; bx offset_y
    mov     dx, word ptr [bp-2]
lampa_poziom:
    cmp     dx, 0
    jb      lampa_dalej3
    cmp     dx, 319
    ja      lampa_dalej3

    mov     di, bx
    add     di, dx
    mov     al, byte ptr [si]
    add     byte ptr es:[di], al
lampa_dalej3:
    inc     si

    inc     dx
    cmp     dx, word ptr [bp-4]
    jne     lampa_poziom
lampa_dalej4:
    inc     cx
    cmp     cx, word ptr [bp-8]
    jne     lampa_pion

    mov     sp, bp
    pop     es
    ret
endp

inicjuj_palete proc
    mov     dx, 03c8h
    xor     ax, ax
    out     dx, al
    inc     dx
    mov     cx, 256
kolor:
    out     dx, al
    out     dx, al
    out     dx, al
    inc     ah
    mov     al, ah
    shr     al, 2
    dec     cx
    jnz     kolor
    ret
endp

generuj_obrazek proc
    push    es
    mov     ax, obrazek_segment
    mov     es, ax
    xor     di, di
    xor     cx, cx
obrazek_pion:
    xor     dx, dx
obrazek_poziom:
    mov     ax, dx
    and     ax, cx
    and     al, 00111111b
    stosb
    inc     dx
    cmp     dx, 320
    jne     obrazek_poziom
    inc     cx
    cmp     cx, 200
    jne     obrazek_pion
    pop     es
    ret
endp

generuj_mape proc
    mov     di, offset mapa
    mov     cx, MAPA_BOK
mapa_pion:
    mov     dx, cx
    mov     cx, MAPA_BOK
mapa_poziom:
    mov     ax, cx
    sub     ax, MAPA_BOK/2
    mov     _fx, ax
    fild    _fx
    fimul   _fx
    fistp   _fx

    mov     ax, dx
    sub     ax, MAPA_BOK/2
    mov     _fy, ax
    fild    _fy
    fimul   _fy

    fiadd    _fx
    fsqrt
    fistp   _fx

    mov     ax, MAPA_BOK/2
    sub     ax, _fx

    cmp     ax, MAPA_BOK/2
    jbe     mapa_dalej1
    xor     ax, ax
mapa_dalej1:
    cmp     ax, 0
    jae     mapa_dalej2
    xor     ax, ax
mapa_dalej2:
    shl     al, 1
    stosb

    dec     cx
    jnz     mapa_poziom
    mov     cx, dx
    dec     cx
    jnz     mapa_pion
    ret
endp

inicjuj_droge proc
    mov     di, offset droga
    xor     cx, cx
droga_skok:
    mov     _fx, cx
    fldpi
    fimul   _fx
    mov     ax, 128
    mov     _fx, ax
    fidiv   _fx
    fsin
    fimul   _fx
    fistp   _fx
    mov     ax, _fx
    stosw
    inc     cx
    cmp     cx, 320
    jne     droga_skok
    ret
endp

.data

obrazek_segment dw 0
_fx dw 0
_fy dw 0

kat_x dw 0
kat_y dw 0

.data?

droga dw 320 dup(?)
linia db 320 dup(?)

mapa db MAPA_BOK*MAPA_BOK dup(?)

end entrypoint
