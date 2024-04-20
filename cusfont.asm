include cusfont.inc
include sys.inc

.model tiny
.code
.386

org 100h

  ; al - ascii code
  ; ah - color
  ; cx - x
  ; dx - y
put_custom_char proc
    sub     al, 32
    cmp     al, 0
    jb      pc_quit
    cmp     al, 63
    ja      pc_quit

    push    es
    mov     es, buffer_seg
    mov     di, dx
    shl     di, 6
    shl     dx, 8
    add     di, dx
    add     di, cx

    mov     dl, ah
    mov     si, offset custom_font
    xor     ah, ah
    shl     ax, 3
    add     si, ax

    mov     ch, 8
pc_loop_y:
    mov     cl, 8
    mov     al, byte ptr [si]
pc_loop_x:
    rcl     al, 1
    jae     pc_next
    mov     es:[di], dl
pc_next:
    inc     di
    dec     cl
    jnz     pc_loop_x
    inc     si
    add     di, 312
    dec     ch
    jnz     pc_loop_y
    pop     es
pc_quit:
    ret
endp

  ; si - offset to null terminated string
  ; cx - x
  ; dx - y
  ; ah - color
put_custom_string proc
    push    es
    mov     es, buffer_seg
    mov     di, dx
    shl     di, 6
    shl     dx, 8
    add     di, dx
    add     di, cx

    mov     dl, ah
ps_char_loop:
    mov     bl, byte ptr [si]
    or      bl, bl
    jz      ps_quit

    sub     bl, 32
    cmp     bl, 0
    jb      ps_next_char
    cmp     bl, 63
    ja      ps_next_char

    xor     bh, bh
    shl     bx, 3
    add     bx, offset custom_font

    push    di
    mov     ch, 8
ps_char_y:
    mov     al, byte ptr [bx]
    mov     cl, 8
ps_char_x:
    rcl     al, 1
    jae     ps_next_pixel
    mov     es:[di], dl
ps_next_pixel:
    inc     di
    dec     cl
    jnz     ps_char_x
    inc     bx
    add     di, 312
    dec     ch
    jnz     ps_char_y
    pop     di

ps_next_char:
    inc     si
    add     di, 8
    jmp     ps_char_loop
ps_quit:
    pop     es
    ret
endp

.data
 
custom_font db     0,    0,    0,    0,    0,    0,    0,    0,   24,   24,   24
 db    24,    8,    0,   24,    0,   40,   40,   40,    0,    0,    0
 db     0,    0,   40,  124,   40,   40,   40,  124,   40,    0,   16
 db   126,   64,  126,  134,  254,   16,    0,  226,  164,  232,   16
 db    46,   74,  142,    0,   56,   40,  124,   68,  196,  206,  252
 db     0,   16,   16,   16,    0,    0,    0,    0,    0,    2,    4
 db    12,    8,   12,    4,    2,    0,  128,   64,   96,   32,   96
 db    64,  128,    0,    0,   84,   56,  108,   56,   84,    0,    0
 db     0,   16,   16,  124,   16,   16,    0,    0,    0,    0,    0
 db     0,    0,   24,    8,    0,    0,    0,    0,   56,    0,    0
 db     0,    0,    0,    0,    0,    0,    0,    0,   24,    0,    4
 db     4,    8,   16,   32,   64,   64,    0,  126,   66,   66,  194
 db   198,  198,  254,    0,   56,    0,    8,   24,   24,   24,   24
 db     0,  124,    4,  124,  192,  192,  198,  254,    0,  126,    2
 db    30,    6,    6,  198,  254,    0,   38,   34,   62,    6,    6
 db     6,    6,    0,  124,   64,  124,    6,    6,  198,  254,    0
 db   120,   64,  126,   70,  198,  198,  254,    0,  254,  134,   12
 db    12,   24,   24,   24,    0,   60,   36,  126,   66,  194,  198
 db   254,    0,  126,   66,  126,    2,    6,  198,  254,    0,    0
 db     0,   24,    0,   24,    0,    0,    0,    0,    0,   24,    0
 db    24,    8,    0,    0,   12,   24,   48,   96,   48,   24,   12
 db     0,    0,    0,  124,    0,  124,    0,    0,    0,   96,   48
 db    24,   12,   24,   48,   96,    0,   28,   54,  102,   12,   24
 db     0,   24,    0,  124,   66,   26,   42,   42,   42,   28,    0
 db    60,   38,  126,   66,  194,  198,  198,    0,  120,   76,  120
 db   204,  198,  198,  252,    0,  126,   66,  192,  192,  192,  198
 db   254,    0,  120,   76,   66,  194,  198,  206,  252,    0,  126
 db    66,   64,  240,  192,  198,  254,    0,  126,   66,   64,  240
 db   192,  192,  192,    0,  126,   70,   64,  192,  206,  198,  254
 db     0,   66,   66,  126,  194,  194,  194,  198,    0,    8,    8
 db     8,   24,   24,   24,   24,    0,    2,    6,    6,    6,  198
 db   198,  254,    0,   72,   80,  112,  216,  204,  198,  198,    0
 db    64,   64,   64,  192,  192,  198,  254,    0,   66,  102,  126
 db   218,  194,  194,  198,    0,   98,   98,   82,  218,  206,  198
 db   198,    0,  126,   66,   66,  194,  198,  198,  254,    0,  124
 db    66,   66,  252,  192,  192,  192,    0,  126,   66,   66,  194
 db   206,  198,  254,    0,  124,   70,  124,  216,  200,  206,  198
 db     0,  126,   64,  126,    6,    6,  198,  254,    0,  126,    8
 db     8,   24,   24,   24,   24,    0,   70,   70,  194,  194,  194
 db   194,  254,    0,   70,   70,   98,   50,   54,   28,   28,    0
 db    70,   70,  194,  210,  254,  238,  198,    0,  198,  108,   56
 db    16,   56,  108,  198,    0,   70,   70,  124,   56,   24,   24
 db    24,    0,  126,    6,   60,   48,  224,  230,  254,    0,   56
 db    32,   32,   32,   32,   32,   56,    0,   64,   64,   32,   16
 db     8,    4,    4,    0,   56,    8,    8,    8,    8,    8,   56
 db     0,   16,   40,   40,    0,    0,    0,    0,    0,    0,    0
 db     0,    0,    0,    0,  254,    0,    0

end
