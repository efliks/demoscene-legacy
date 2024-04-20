include tex3.inc
include sys.inc

SHIFT_CONST equ 12

.model tiny
.code
.386

org 100h


textured_triangle proc

@@x1 equ word ptr [bp+4]
@@y1 equ word ptr [bp+6]
@@x2 equ word ptr [bp+8]
@@y2 equ word ptr [bp+10]
@@x3 equ word ptr [bp+12]
@@y3 equ word ptr [bp+14]

@@tex_off equ word ptr [bp+16]
@@tex_seg equ word ptr [bp+18]

@@tex_x1 equ word ptr [bp+20]
@@tex_y1 equ word ptr [bp+22]
@@tex_x2 equ word ptr [bp+24]
@@tex_y2 equ word ptr [bp+26]
@@tex_x3 equ word ptr [bp+28]
@@tex_y3 equ word ptr [bp+30]

@@dx12 equ dword ptr [bp-4]
@@dx13 equ dword ptr [bp-8]
@@dx23 equ dword ptr [bp-12]

@@tex_dx12 equ dword ptr [bp-16]
@@tex_dy12 equ dword ptr [bp-20]
@@tex_dx13 equ dword ptr [bp-24]
@@tex_dy13 equ dword ptr [bp-28]
@@tex_dx23 equ dword ptr [bp-32]
@@tex_dy23 equ dword ptr [bp-36]

@@scan_x1 equ dword ptr [bp-40]
@@scan_y1 equ dword ptr [bp-44]
@@scan_x2 equ dword ptr [bp-48]
@@scan_y2 equ dword ptr [bp-52]

    push    bp
    mov     bp, sp
    sub     sp, 52

    push    ds
    push    es
    mov     es, buffer_seg
    mov     ds, @@tex_seg

    mov     ax, @@y1
    cmp     ax, @@y3
    jl      tt_check1

    xchg    ax, @@y3
    mov     @@y1, ax

    mov     ax, @@x1
    xchg    ax, @@x3
    mov     @@x1, ax

    mov     ax, @@tex_y1
    xchg    ax, @@tex_y3
    mov     @@tex_y1, ax

    mov     ax, @@tex_x1
    xchg    ax, @@tex_x3
    mov     @@tex_x1, ax
tt_check1:
    mov     ax, @@y2
    cmp     ax, @@y3
    jl      tt_check2

    xchg    ax, @@y3
    mov     @@y2, ax

    mov     ax, @@x2
    xchg    ax, @@x3
    mov     @@x2, ax

    mov     ax, @@tex_y2
    xchg    ax, @@tex_y3
    mov     @@tex_y2, ax

    mov     ax, @@tex_x2
    xchg    ax, @@tex_x3
    mov     @@tex_x2, ax
tt_check2:
    mov     ax, @@y1
    cmp     ax, @@y2
    jl      tt_check3

    xchg    ax, @@y2
    mov     @@y1, ax

    mov     ax, @@x1
    xchg    ax, @@x2
    mov     @@x1, ax

    mov     ax, @@tex_y1
    xchg    ax, @@tex_y2
    mov     @@tex_y1, ax

    mov     ax, @@tex_x1
    xchg    ax, @@tex_x2
    mov     @@tex_x1, ax
tt_check3:


    mov     bx, @@y2
    sub     bx, @@y1
    jnz     tt_dx12_make

    mov     @@dx12, 0
    mov     @@tex_dx12, 0
    mov     @@tex_dy12, 0
    jmp     tt_dx12_done
tt_dx12_make:
    mov     ax, @@x2
    sub     ax, @@x1
    movsx   eax, ax
    movsx   ebx, bx
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@dx12, eax  ; dx12 = (x2-x1)/(y2-y1)

    mov     ax, @@tex_x2
    sub     ax, @@tex_x1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dx12, eax  ; tex_dx12 = (tex_x2-tex_x1)/(y2-y1)

    mov     ax, @@tex_y2
    sub     ax, @@tex_y1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dy12, eax  ; tex_dy12 = (tex_y2-tex_y1)/(y2-y1)
tt_dx12_done:


    mov     bx, @@y3
    sub     bx, @@y1
    jnz     tt_dx13_make

    mov     @@dx13, 0
    mov     @@tex_dx13, 0
    mov     @@tex_dy13, 0
    jmp     tt_dx13_done
tt_dx13_make:
    mov     ax, @@x3
    sub     ax, @@x1
    movsx   eax, ax
    movsx   ebx, bx
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@dx13, eax  ; dx13 = (x3-x1)/(y3-y1)

    mov     ax, @@tex_x3
    sub     ax, @@tex_x1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dx13, eax  ; tex_dx13 = (tex_x3-tex_x1)/(y3-y1)

    mov     ax, @@tex_y3
    sub     ax, @@tex_y1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dy13, eax  ; tex_dy13 = (tex_y3-tex_y1)/(y3-y1)
tt_dx13_done:


    mov     bx, @@y3
    sub     bx, @@y2
    jnz     tt_dx23_make

    mov     @@dx23, 0
    mov     @@tex_dx23, 0
    mov     @@tex_dy23, 0
    jmp     tt_dx23_done
tt_dx23_make:
    mov     ax, @@x3
    sub     ax, @@x2
    movsx   eax, ax
    movsx   ebx, bx
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@dx23, eax  ; dx23 = (x3-x2)/(y3-y2)

    mov     ax, @@tex_x3
    sub     ax, @@tex_x2
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dx23, eax  ; tex_dx23 = (tex_x3-tex_x2)/(y3-y2)

    mov     ax, @@tex_y3
    sub     ax, @@tex_y2
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ebx
    mov     @@tex_dy23, eax  ; tex_dy23 = (tex_y3-tex_y2)/(y3-y2)
tt_dx23_done:


    movsx   eax, @@x1
    shl     eax, SHIFT_CONST
    mov     ebx, eax

    movsx   edx, @@tex_x1
    shl     edx, SHIFT_CONST
    mov     @@scan_x1, edx
    mov     @@scan_x2, edx
    movsx   edx, @@tex_y1
    shl     edx, SHIFT_CONST
    mov     @@scan_y1, edx
    mov     @@scan_y2, edx

    mov     cx, @@y1
    cmp     cx, @@y2
    jge     tt_loop1_end
tt_loop1:
    push    eax
    push    ebx
    push    cx

    mov     edx, @@scan_y2
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_x2
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_y1
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_x1
    sar     edx, SHIFT_CONST
    push    dx
    push    @@tex_off

    push    cx
    mov     edx, ebx
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, eax
    sar     edx, SHIFT_CONST
    push    dx
    call    textured_line

    pop     cx
    pop     ebx
    pop     eax

    mov     edx, @@tex_dx13
    add     @@scan_x1, edx
    mov     edx, @@tex_dx12
    add     @@scan_x2, edx
    mov     edx, @@tex_dy13
    add     @@scan_y1, edx
    mov     edx, @@tex_dy12
    add     @@scan_y2, edx

    add     eax, @@dx13
    add     ebx, @@dx12
    inc     cx
    cmp     cx, @@y2
    jl      tt_loop1
tt_loop1_end:


    mov     cx, @@y2
    cmp     cx, @@y3
    jge     tt_loop2_end

    movsx   ebx, @@x2
    shl     ebx, SHIFT_CONST

    movsx   edx, @@tex_x2
    shl     edx, SHIFT_CONST
    mov     @@scan_x2, edx
    movsx   edx, @@tex_y2
    shl     edx, SHIFT_CONST
    mov     @@scan_y2, edx
tt_loop2:
    push    eax
    push    ebx
    push    cx

    mov     edx, @@scan_y2
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_x2
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_y1
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, @@scan_x1
    sar     edx, SHIFT_CONST
    push    dx
    push    @@tex_off

    push    cx
    mov     edx, ebx
    sar     edx, SHIFT_CONST
    push    dx
    mov     edx, eax
    sar     edx, SHIFT_CONST
    push    dx
    call    textured_line

    pop     cx
    pop     ebx
    pop     eax

    mov     edx, @@tex_dx13
    add     @@scan_x1, edx
    mov     edx, @@tex_dx23
    add     @@scan_x2, edx
    mov     edx, @@tex_dy13
    add     @@scan_y1, edx
    mov     edx, @@tex_dy23
    add     @@scan_y2, edx

    add     eax, @@dx13
    add     ebx, @@dx23
    inc     cx
    cmp     cx, @@y3
    jl      tt_loop2
tt_loop2_end:

    pop     es
    pop     ds

    mov     sp, bp
    pop     bp
    ret     28
endp

textured_line proc

@@x1 equ word ptr [bp+4]
@@x2 equ word ptr [bp+6]
@@y equ word ptr [bp+8]

@@tex_off equ word ptr [bp+10]
@@tex_x1 equ word ptr [bp+12]
@@tex_y1 equ word ptr [bp+14]
@@tex_x2 equ word ptr [bp+16]
@@tex_y2 equ word ptr [bp+18]

@@tex_dx equ dword ptr [bp-4]
@@tex_dy equ dword ptr [bp-8]

    push    bp
    mov     bp, sp
    sub     sp, 8

    mov     ax, @@y
    or      ax, ax
    jl      tl_quit
    cmp     ax, 199
    jg      tl_quit

    mov     ax, @@x1
    cmp     ax, @@x2
    je      tl_quit
    jl      tl_ok

    xchg    ax, @@x2
    mov     @@x1, ax

    mov     ax, @@tex_x1
    xchg    ax, @@tex_x2
    mov     @@tex_x1, ax

    mov     ax, @@tex_y1
    xchg    ax, @@tex_y2
    mov     @@tex_y1, ax
tl_ok:
    mov     di, @@y
    mov     ax, di
    shl     di, 6
    shl     ax, 8
    add     di, ax
    add     di, @@x1

    mov     cx, @@x2
    sub     cx, @@x1
    movsx   ecx, cx

    mov     ax, @@tex_x2
    sub     ax, @@tex_x1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ecx
    mov     @@tex_dx, eax  ; tex_dx = (tex_x2-tex_x1)/(x2-x1)

    mov     ax, @@tex_y2
    sub     ax, @@tex_y1
    movsx   eax, ax
    shl     eax, SHIFT_CONST
    cdq
    idiv    ecx
    mov     @@tex_dy, eax  ; tex_dy = (tex_y2-tex_y1)/(x2-x1)

    movsx   eax, @@tex_x1
    shl     eax, SHIFT_CONST
    movsx   ebx, @@tex_y1
    shl     ebx, SHIFT_CONST
tl_loop:
    mov     edx, eax
    mov     esi, ebx
    sar     edx, SHIFT_CONST
    sar     esi, SHIFT_CONST
    shl     si, 7
    add     si, dx
    add     si, @@tex_off
    movsb

    add     eax, @@tex_dx
    add     ebx, @@tex_dy
    dec     ecx
    jnz     tl_loop
tl_quit:

    mov     sp, bp
    pop     bp
    ret     16
endp

end
