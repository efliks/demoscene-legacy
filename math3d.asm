include math3d.inc

.model tiny
.code
.386

org 100h


init_sincos proc
    push    bp
    mov     bp, sp
    sub     sp, 4

    mov     si, offset sintab
    mov     di, offset costab
    mov     dword ptr [bp-4], 0
    mov     cx, MAX_DEGS
isc_loop:
    fld     dword ptr [bp-4]
    fld     st
    fld     st
    fsin
    fstp    dword ptr [si]
    fcos
    fstp    dword ptr [di]
    fadd    delta_angle
    fstp    dword ptr [bp-4]
    add     si, 4
    add     di, 4
    dec     cx
    jnz     isc_loop

    mov     sp, bp
    pop     bp
    ret
endp

;------------------------------------------------------------
;    in:    si - offset to angles (word)
;        di - offset to 3x3 matrix
;    out:    none
;------------------------------------------------------------

mx_rotation_matrix proc

@@sin_x equ dword ptr [bp-4]
@@cos_x equ dword ptr [bp-8]
@@sin_y equ dword ptr [bp-12]
@@cos_y equ dword ptr [bp-16]
@@sin_z equ dword ptr [bp-20]
@@cos_z equ dword ptr [bp-24]

    push    bp
    mov     bp, sp
    sub     sp, 24

    mov     bx, word ptr [si]
    and     bx, MAX_DEGS-1
    shl     bx, 2
    mov     eax, dword ptr sintab[bx]
    mov     @@sin_x, eax
    mov     eax, dword ptr costab[bx]
    mov     @@cos_x, eax

    mov     bx, word ptr [si+2]
    and     bx, MAX_DEGS-1
    shl     bx, 2
    mov     eax, dword ptr sintab[bx]
    mov     @@sin_y, eax
    mov     eax, dword ptr costab[bx]
    mov     @@cos_y, eax

    mov     bx, word ptr [si+4]
    and     bx, MAX_DEGS-1
    shl     bx, 2
    mov     eax, dword ptr sintab[bx]
    mov     @@sin_z, eax
    mov     eax, dword ptr costab[bx]
    mov     @@cos_z, eax


    fld     @@cos_y
    fmul    @@cos_z
    fstp    dword ptr [di]

    fld     @@sin_x
    fmul    @@sin_y
    fmul    @@cos_z
    fld     @@cos_x
    fchs
    fmul    @@sin_z
    faddp   st(1), st
    fstp    dword ptr [di+12]

    fld     @@cos_x
    fmul    @@sin_y
    fmul    @@cos_z
    fld     @@sin_x
    fmul    @@sin_z
    faddp   st(1), st
    fstp    dword ptr [di+24]

    fld     @@cos_y
    fmul    @@sin_z
    fstp    dword ptr [di+4]

    fld     @@sin_x
    fmul    @@sin_y
    fmul    @@sin_z
    fld     @@cos_x
    fmul    @@cos_z
    faddp   st(1), st
    fstp    dword ptr [di+16]

    fld     @@cos_x
    fmul    @@sin_y
    fmul    @@sin_z
    fld     @@sin_x
    fchs
    fmul    @@cos_z
    faddp   st(1), st
    fstp    dword ptr [di+28]

    fld     @@sin_y
    fchs
    fstp    dword ptr [di+8]

    fld     @@cos_y
    fmul    @@sin_x
    fstp    dword ptr [di+20]

    fld     @@cos_x
    fmul    @@cos_y
    fstp    dword ptr [di+32]

    mov     sp, bp
    pop     bp
    ret
endp

;------------------------------------------------------------
;    in:    si - offset to points
;        di - offset to rotated_points
;        bx - offset to 3x3 rotation matrix
;        cx - number of points
;    out:    none
;------------------------------------------------------------

mx_rotate_points proc

    fld     dword ptr [si.x3d]
    fmul    dword ptr [bx]
    fld     dword ptr [si.y3d]
    fmul    dword ptr [bx+12]
    faddp   st(1), st
    fld     dword ptr [si.z3d]
    fmul    dword ptr [bx+24]
    faddp   st(1), st
    fstp    dword ptr [di.x3d]

    fld     dword ptr [si.x3d]
    fmul    dword ptr [bx+4]
    fld     dword ptr [si.y3d]
    fmul    dword ptr [bx+16]
    faddp   st(1), st
    fld     dword ptr [si.z3d]
    fmul    dword ptr [bx+28]
    faddp   st(1), st
    fstp    dword ptr [di.y3d]

    fld     dword ptr [si.x3d]
    fmul    dword ptr [bx+8]
    fld     dword ptr [si.y3d]
    fmul    dword ptr [bx+20]
    faddp   st(1), st
    fld     dword ptr [si.z3d]
    fmul    dword ptr [bx+32]
    faddp   st(1), st
    fstp    dword ptr [di.z3d]

    add     si, 12
    add     di, 12
    dec     cx
    jnz     mx_rotate_points

    ret
endp

;------------------------------------------------------------
;    in:    si - offset to 3d-points
;        di - offset to 2d-points
;        cx - number of points
;    out:    none
;------------------------------------------------------------

translate_points proc

    fld     dword ptr [si.z3d]
    fadd    perspective

    fld     dword ptr [si.x3d]
    fmul    perspective
    fdiv    st, st(1)
    fadd    correct_x
    fistp   word ptr [di.x2d]  ; x2d = (x*persp)/(z+persp)+correct_x

    fld     dword ptr [si.y3d]
    fmul    perspective
    fdivrp  st(1), st
    fadd    correct_y
    fistp   word ptr [di.y2d]  ; y2d = (y*persp)/(z+persp)+correct_y

    add     si, type point3d
    add     di, type point2d
    dec     cx
        jnz    translate_points

    ret
endp

;------------------------------------------------------------
;    in:    di - offset to vector
;    out:    none
;------------------------------------------------------------

normalize_vector proc

    fld     dword ptr [di.vec_x]
    fmul    st, st
    fld     dword ptr [di.vec_y]
    fmul    st, st
    fld     dword ptr [di.vec_z]
    fmul    st, st
    faddp   st(1), st
    faddp   st(1), st
    fsqrt  ; st - lenght

    ftst
    fstsw   ax
    sahf
    jnz     nv_not_zero

    fst     dword ptr [di.vec_x]
    fst     dword ptr [di.vec_y]
    fstp    dword ptr [di.vec_z]
    ret
nv_not_zero:
    fld     st
    fld     st
    fdivr   dword ptr [di.vec_x]
    fstp    dword ptr [di.vec_x]
    fdivr   dword ptr [di.vec_y]
    fstp    dword ptr [di.vec_y]
    fdivr   dword ptr [di.vec_z]
    fstp    dword ptr [di.vec_z]

    ret
endp

;------------------------------------------------------------
;    in:    si - offset to 1st vector
;        di - offset to 2nd vector
;    out:    st(0) - dot-product
;------------------------------------------------------------

dot_product proc

    fld     dword ptr [si.vec_x]
    fmul    dword ptr [di.vec_x]
    fld     dword ptr [si.vec_y]
    fmul    dword ptr [di.vec_y]
    fld     dword ptr [si.vec_z]
    fmul    dword ptr [di.vec_z]
    faddp   st(1), st
    faddp   st(1), st

    ret
endp

;------------------------------------------------------------
;    in:    si - offset to 1st vector
;        di - offset to 2nd vector
;        bx - offset to result vector
;    out:    none
;------------------------------------------------------------

cross_product proc

    fld     dword ptr [si.vec_y]
    fmul    dword ptr [di.vec_z]
    fld     dword ptr [si.vec_z]
    fmul    dword ptr [di.vec_y]
    fsubp   st(1), st
    fstp    dword ptr [bx.vec_x]

    fld     dword ptr [si.vec_z]
    fmul    dword ptr [di.vec_x]
    fld     dword ptr [si.vec_x]
    fmul    dword ptr [di.vec_z]
    fsubp   st(1), st
    fstp    dword ptr [bx.vec_y]

    fld     dword ptr [si.vec_x]
    fmul    dword ptr [di.vec_y]
    fld     dword ptr [si.vec_y]
    fmul    dword ptr [di.vec_x]
    fsubp   st(1), st
    fstp    dword ptr [bx.vec_z]

    ret
endp

;------------------------------------------------------------
;    in:    si - offset to 1st 3d point
;        di - offset to 2nd 3d point
;        bx - offset to result vector
;    out:    none
;------------------------------------------------------------

make_vector proc

    fld     dword ptr [di.x3d]
    fsub    dword ptr [si.x3d]
    fstp    dword ptr [bx.vec_x]

    fld     dword ptr [di.y3d]
    fsub    dword ptr [si.y3d]
    fstp    dword ptr [bx.vec_y]

    fld     dword ptr [di.z3d]
    fsub    dword ptr [si.z3d]
    fstp    dword ptr [bx.vec_z]

    ret
endp
 
  ; si - offset to angles
  ; di - offset to array of 3d points
  ; cx - number of points
rotate_points_old proc
    push    bp
    mov     bp, sp
    sub     sp, 24
    
    ; bp-4  - sin_x
    ; bp-8  - cos_x
    ; bp-12 - sin_y
    ; bp-16 - cos_y
    ; bp-20 - sin_z
    ; bp-24 - cos_z
    push    di
    push    cx

    mov     di, bp
    sub     di, 4
    mov     cx, 3
rp_sin_cos:
    fld     dword ptr [si]
    fld     st(0)
    fsin
    fstp    dword ptr [di]
    fcos
    fstp    dword ptr [di-4]
    sub     di, 8
    add     si, 4
    dec     cx
    jnz     rp_sin_cos

    pop     cx
    pop     si
rp_rotate:
    fld     dword ptr [si+4]  ; y
    fmul    dword ptr [bp-8]  ; cos_x
    fld     dword ptr [si+8]  ; z
    fmul    dword ptr [bp-4]  ; sin_x
    fsubp   st(1), st
    fld     dword ptr [si+4]  ; y
    fxch    st(1)
    fstp    dword ptr [si+4]  ; Yrotated = Cos (A)*Y - Sin (A)*Z
    fmul    dword ptr [bp-4]  ; sin_x
    fld     dword ptr [si+8]  ; z
    fmul    dword ptr [bp-8]  ; cos_x
    faddp   st(1), st
    fstp    dword ptr [si+8]  ; Zrotated = Sin (A)*Y + Cos (A)*Z

    fld     dword ptr [si]  ; x
    fmul    dword ptr [bp-16]  ; cos_y
    fld     dword ptr [si+8]  ; z
    fmul    dword ptr [bp-12]  ; sin_y
    fsubp   st(1), st
    fld     dword ptr [si]  ; x
    fxch    st(1)
    fstp    dword ptr [si]  ; Xrotated = Cos (A)*X - Sin (A)*Z
    fmul    dword ptr [bp-12]  ; sin_y
    fld     dword ptr [si+8]  ; z
    fmul    dword ptr [bp-16]  ; cos_y
    faddp   st(1), st
    fstp    dword ptr [si+8]  ; Zrotated = Sin (A)*X + Cos (A)*Z

    fld     dword ptr [si]  ; x
    fmul    dword ptr [bp-24]  ; cos_z
    fld     dword ptr [si+4]  ; y
    fmul    dword ptr [bp-20]  ; sin_z
    fsubp   st(1), st
    fld     dword ptr [si]  ; x
    fxch    st(1)
    fstp    dword ptr [si]  ; Xrotated = Cos (A)*X - Sin (A)*Y
    fmul    dword ptr [bp-20]  ; sin_z
    fld     dword ptr [si+4]  ; y
    fmul    dword ptr [bp-24]  ; cos_z
    faddp   st(1), st
    fstp    dword ptr [si+4]  ; Yrotated = Sin (A)*X + Cos (A)*Y

    add     si, 12
    dec     cx
    jnz     rp_rotate

    add     sp, 24
    pop     bp
    ret
endp

.data

perspective dd 256.0
correct_x dd 160.0
correct_y dd 100.0

  ; pi/(MAX_DEGS/2)
delta_angle dd 0.0122718463031

.data? 

sintab dd MAX_DEGS dup(?)
costab dd MAX_DEGS dup(?)

end
