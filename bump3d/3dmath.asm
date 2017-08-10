.386p
locals

b	equ byte ptr
w	equ word ptr
d	equ dword ptr
o	equ offset

code32 segment para public use32
	assume cs:code32,  ds:code32

;------------------------------------------------------------
;	in:	esi - offset to angles (float type!)
;		edi - offset to 3x3 matrix
;	out:	none
;------------------------------------------------------------

mx_rotation_matrix proc

	mov	ebx, o sin_x
	mov	ecx, 3
@@load:
	fld	d [esi]
	fld	st
	fsin
	fstp	d [ebx]
	fcos
	fstp	d [ebx + 4]

	add	esi, 4
	add	ebx, 8
	dec	ecx
	jnz	@@load

	fld	d [cos_y]
	fmul	d [cos_z]
	fstp	d [edi + 0 * 4 + 0 * 12]

	fld	d [sin_x]
	fmul	d [sin_y]
	fmul	d [cos_z]
	fld	d [cos_x]
	fchs
	fmul	d [sin_z]
	faddp	st(1), st(0)
	fstp	d [edi + 0 * 4 + 1 * 12]

	fld	d [cos_x]
	fmul	d [sin_y]
	fmul	d [cos_z]
	fld	d [sin_x]
	fmul	d [sin_z]
	faddp	st(1), st(0)
	fstp	d [edi + 0 * 4 + 2 * 12]

	fld	d [cos_y]
	fmul	d [sin_z]
	fstp	d [edi + 1 * 4 + 0 * 12]

	fld	d [sin_x]
	fmul	d [sin_y]
	fmul	d [sin_z]
	fld	d [cos_x]
	fmul	d [cos_z]
	faddp	st(1), st(0)
	fstp	d [edi + 1 * 4 + 1 * 12]

	fld	d [cos_x]
	fmul	d [sin_y]
	fmul	d [sin_z]
	fld	d [sin_x]
	fchs
	fmul	d [cos_z]
	faddp	st(1), st(0)
	fstp	d [edi + 1 * 4 + 2 * 12]

	fld	d [sin_y]
	fchs
	fstp	d [edi + 2 * 4 + 0 * 12]

	fld	d [cos_y]
	fmul	d [sin_x]
	fstp	d [edi + 2 * 4 + 1 * 12]

	fld	d [cos_x]
	fmul	d [cos_y]
	fstp	d [edi + 2 * 4 + 2 * 12]

comment #
	fld	mx_cos_y
	fmul	mx_cos_z
	fstp	d [edi.m_00]

	fld	mx_sin_x
	fmul	mx_sin_y
	fmul	mx_cos_z
	fld	mx_cos_x
	fchs
	fmul	mx_sin_z
	faddp	st(1), st
	fstp	d [edi.m_10]

	fld	mx_cos_x
	fmul	mx_sin_y
	fmul	mx_cos_z
	fld	mx_sin_x
	fmul	mx_sin_z
	faddp	st(1), st
	fstp	d [edi.m_20]

	fld	mx_cos_y
	fmul	mx_sin_z
	fstp	d [edi.m_01]

	fld	mx_sin_x
	fmul	mx_sin_y
	fmul	mx_sin_z
	fld	mx_cos_x
	fmul	mx_cos_z
	faddp	st(1), st
	fstp	d [edi.m_11]

	fld	mx_cos_x
	fmul	mx_sin_y
	fmul	mx_sin_z
	fld	mx_sin_x
	fchs
	fmul	mx_cos_z
	faddp	st(1), st
	fstp	d [edi.m_21]

	fld	mx_sin_y
	fchs
	fstp	d [edi.m_02]

	fld	mx_cos_y
	fmul	mx_sin_x
	fstp	d [edi.m_12]

	fld	mx_cos_x
	fmul	mx_cos_y
	fstp	d [edi.m_22] #

	ret
endp

;------------------------------------------------------------
;	in:	esi - offset to points
;		edi - offset to rotated_points
;		ebx - offset to 3x3 rotation matrix
;		ecx - number of points
;	out:	none
;------------------------------------------------------------

mx_rotate_points proc

comment #
	fld	d [esi.x3d]
	fmul	d [ebx.m_00]
	fld	d [esi.y3d]
	fmul	d [ebx.m_10]
	fld	d [esi.z3d]
	fmul	d [ebx.m_20]
	faddp	st(1), st
	faddp	st(1), st
	fstp	d [edi.x3d]

	fld	d [esi.x3d]
	fmul	d [ebx.m_01]
	fld	d [esi.y3d]
	fmul	d [ebx.m_11]
	fld	d [esi.z3d]
	fmul	d [ebx.m_21]
	faddp	st(1), st
	faddp	st(1), st
	fstp	d [edi.y3d]

	fld	d [esi.x3d]
	fmul	d [ebx.m_02]
	fld	d [esi.y3d]
	fmul	d [ebx.m_12]
	fld	d [esi.z3d]
	fmul	d [ebx.m_22]
	faddp	st(1), st
	faddp	st(1), st
	fstp	d [edi.z3d] #

	fld	d [esi + 0]
	fmul	d [ebx + 0 * 4 + 0 * 12]
	fld	d [esi + 4]
	fmul	d [ebx + 0 * 4 + 1 * 12]
	fld	d [esi + 8]
	fmul	d [ebx + 0 * 4 + 2 * 12]
	faddp	st(1), st(0)
	faddp	st(1), st(0)
	fstp	d [edi + 0]

	fld	d [esi + 0]
	fmul	d [ebx + 1 * 4 + 0 * 12]
	fld	d [esi + 4]
	fmul	d [ebx + 1 * 4 + 1 * 12]
	fld	d [esi + 8]
	fmul	d [ebx + 1 * 4 + 2 * 12]
	faddp	st(1), st(0)
	faddp	st(1), st(0)
	fstp	d [edi + 4]

	fld	d [esi + 0]
	fmul	d [ebx + 2 * 4 + 0 * 12]
	fld	d [esi + 4]
	fmul	d [ebx + 2 * 4 + 1 * 12]
	fld	d [esi + 8]
	fmul	d [ebx + 2 * 4 + 2 * 12]
	faddp	st(1), st(0)
	faddp	st(1), st(0)
	fstp	d [edi + 8]

	add	esi, size point3d
	add	edi, size point3d
	dec	ecx
	jnz	mx_rotate_points

	ret
endp

;------------------------------------------------------------
;	in:	esi - offset to 3d points
;		edi - offset to 2d points
;		ecx - number of points
;	out:	none
;------------------------------------------------------------

translate_points proc

	fld	d [esi.z3d]
	fadd	perspective

	fld	d [esi.x3d]
	fmul	perspective
	fdiv	st, st(1)
	fadd	correct_x
	fistp	w [edi.x2d]

	fld	d [esi.y3d]
	fmul	perspective
	fdivrp	st(1), st
	fadd	correct_y
	fistp	w [edi.y2d]

	add	esi, size point3d
	add	edi, size point2d
	dec	ecx
	jnz	translate_points

	ret
endp

;------------------------------------------------------------
;	in:	edi - offset to vector
;	out:	none
;------------------------------------------------------------

normalize_vector proc

	push	eax
	fld	d [edi + 0]
	fmul	st(0), st(0)
	fld	d [edi + 4]
	fmul	st(0), st(0)
	fld	d [edi + 8]
	fmul	st(0), st(0)
	faddp	st(1), st(0)
	faddp	st(1), st(0)
	fsqrt
	ftst
	fstsw	ax
	sahf
	jz	NV_zero
	fld	d [edi + 0]
	fdiv	st(0), st(1)
	fstp	d [edi + 0]
	fld	d [edi + 4]
	fdiv	st(0), st(1)
	fstp	d [edi + 4]
	fld	d [edi + 8]
	fdivrp	st(1), st(0)
	fstp	d [edi + 8]
	pop	eax
	ret
NV_zero:
	ffree	st
	xor	eax, eax
	stosd
	stosd
	stosd
	pop	eax
	ret
endp

comment #
	push	eax

	fld	d [edi.vec_x]
	fmul	st, st
	fld	d [edi.vec_y]
	fmul	st, st
	fld	d [edi.vec_z]
	fmul	st, st
	faddp	st(1), st
	faddp	st(1), st
	fsqrt
	
	ftst
	fstsw	ax
	sahf
	jnz	@@not_zero

	fst	d [edi.vec_x]
	fst	d [edi.vec_y]
	fstp	d [edi.vec_z]
	jmp	@@quit

@@not_zero:
	fld	st
	fld	st
	fdivr	d [edi.vec_x]
	fstp	d [edi.vec_x]
	fdivr	d [edi.vec_y]
	fstp	d [edi.vec_y]
	fdivr	d [edi.vec_z]
	fstp	d [edi.vec_z]

@@quit:
	pop	eax
	ret
endp #

comment #
;------------------------------------------------------------
;	in:	esi - offset to 1st vector
;		edi - offset to 2nd vector
;	out:	st(0) - dot-product
;------------------------------------------------------------

dot_product proc

	fld	d [esi.vec_x]
	fmul	d [edi.vec_x]
	fld	d [esi.vec_y]
	fmul	d [edi.vec_y]
	fld	d [esi.vec_z]
	fmul	d [edi.vec_z]
	faddp	st(1), st
	faddp	st(1), st

	ret
endp #

;------------------------------------------------------------
;	in:	esi - offset to 1st vector
;		edi - offset to 2nd vector
;		ebx - offset to result vector
;	out:	none
;------------------------------------------------------------

cross_product proc

	fld	d [esi.vec_y]
	fmul	d [edi.vec_z]
	fld	d [esi.vec_z]
	fmul	d [edi.vec_y]
	fsubp	st(1), st
	fstp	d [ebx.vec_x]

	fld	d [esi.vec_z]
	fmul	d [edi.vec_x]
	fld	d [esi.vec_x]
	fmul	d [edi.vec_z]
	fsubp	st(1), st
	fstp	d [ebx.vec_y]

	fld	d [esi.vec_x]
	fmul	d [edi.vec_y]
	fld	d [esi.vec_y]
	fmul	d [edi.vec_x]
	fsubp	st(1), st
	fstp	d [ebx.vec_z]

	ret
endp

;------------------------------------------------------------
;	in:	esi - offset to 1st 3d point
;		edi - offset to 2nd 3d point
;		ebx - offset to result vector
;	out:	none
;------------------------------------------------------------

make_vector proc

	fld	d [edi.x3d]
	fsub	d [esi.x3d]
	fstp	d [ebx.vec_x]

	fld	d [edi.y3d]
	fsub	d [esi.y3d]
	fstp	d [ebx.vec_y]

	fld	d [edi.z3d]
	fsub	d [esi.z3d]
	fstp	d [ebx.vec_z]

	ret
endp

perspective	dd 256.0
correct_x	dd 160.0
correct_y	dd 100.0

sin_x	dd ?
cos_x	dd ?
sin_y	dd ?
cos_y	dd ?
sin_z	dd ?
cos_z	dd ?

code32 ends
