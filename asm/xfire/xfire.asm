; Originally written 1/05/2001

title 'xfire.asm', 'Fire simulation'

include globals.inc
include graphlib.inc


_STACK  segment
    DW  100h dup (?)
    EndStack    LABEL  BYTE
_STACK  ends


_DATA   segment
include fire_pal.inc
random_seed     dw  1111h

_DATA   ends


_TEXT   segment
.386

xfire_start:
    lea     bx, EndStack     ;end of program
    mov     cl, 4
    shr     bx, cl          ;adjust to paragraph
    inc     bx              ;round up
    mov     ax, ss
    add     bx, ax
    mov     ax, ds
    sub     bx, ax
    mov     ah, 4ah
    int     21h

    ;    initialize
    mov     dx, DGROUP

    call    alloc_screen
    call    clear_screen

    mov     ax, 13h
    int     10h

    push    ds
    mov     ax, DGROUP
    mov     ds, ax
    mov     si, offset fire_pal
    mov     cx, 768
    xor     ax, ax
    mov     dx, 03c8h
    out     dx, al
    inc     dx
    cld
    rep     outsb
    pop     ds

main_loop:
    call    burn
    call    wait_for_frame
    call    copy_screen
    in      al, 60h
    dec     al
    jnz     main_loop

    mov     ax, 03h
    int     10h
    call    free_screen

    mov     ax, 4c00h
    int     21h
    ret


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
endp


burn proc
    push   ds
    mov    ds, buffer_seg

    mov    di, 64000
    mov    cx, 80
burn_loop1:
    mov    bx, ds:[random_seed]
    add    bx, 9248h
    ror    bx, 3
    mov    ds:[random_seed], bx
    mov    ax, 255  ; max colors
    mul    bx
    mov    ax, dx
    
    mov    ah, al
    mov    dx, ax
    shl    eax, 16
    mov    ax, dx
    mov    dword ptr [di], eax
    mov    dword ptr [di+320], eax
    mov    dword ptr [di+640], eax
    mov    dword ptr [di+960], eax

    add    di, 4
    dec    cx
    jnz    burn_loop1

    mov    di, 640
    mov    cx, 64000+640
    xor    ax, ax
    xor    bx, bx
burn_loop2:
    mov    al, byte ptr [di-1]
    mov    bl, byte ptr [di+1]
    add    ax, bx
    mov    bl, byte ptr [di-320]
    add    ax, bx
    mov    bl, byte ptr [di+320]
    add    ax, bx
    shr    ax, 2
    dec    al
    jz     burn_next
    mov    byte ptr [di-320], al
    mov    byte ptr [di-640], al
burn_next:
    inc    di
    dec    cx
    jnz    burn_loop2

    pop    ds
    ret
endp

_TEXT    ends

END     xfire_start

