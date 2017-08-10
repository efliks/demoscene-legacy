.386p
.model flat

.code

; in: edi = ptr to struct3d
CenterStruct3d:
	fldz
	fst	q [str3d_ox]
	fst	q [str3d_oy]
	fstp	q [str3d_oz]


	mov	ebp, d [edi.s3d_points]
	mov	ecx, d [edi.s3d_n_points]
CS3d_add:
	fld	q [str3d_ox]
	fadd	d [ebp.x3d]
	fstp	q [str3d_ox]

	fld	q [str3d_oy]
	fadd	d [ebp.y3d]
	fstp	q [str3d_oy]

	fld	q [str3d_oz]
	fadd	d [ebp.z3d]
	fstp	q [str3d_oz]
	add	ebp, size point3d
	loop	CS3d_add


	fld	q [str3d_ox]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_ox]

	fld	q [str3d_oy]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_oy]

	fld	q [str3d_oz]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_oz]


	mov	ebp, d [edi.s3d_points]
	mov	ecx, d [edi.s3d_n_points]
CS3d_move:
	fld	d [ebp.x3d]
	fsub	q [str3d_ox]
	fstp	d [ebp.x3d]

	fld	d [ebp.y3d]
	fsub	q [str3d_oy]
	fstp	d [ebp.y3d]

	fld	d [ebp.z3d]
	fsub	q [str3d_oz]
	fstp	d [ebp.z3d]

	add	ebp, size point3d
	loop	CS3d_move
	ret

;------------------------------------------------------------

; in: edi = ptr to struct3d, filled ScaleX, ScaleY, ScaleZ 
ScaleStruct3d:
	mov	ecx, d [edi.s3d_n_points]
	mov	ebx, d [edi.s3d_points]
SS3d_scale:
	fld	d [ebx.x3d]
	fmul	d [ScaleX]
	fstp	d [ebx.x3d]

	fld	d [ebx.y3d]
	fmul	d [ScaleY]
	fstp	d [ebx.y3d]

	fld	d [ebx.z3d]
	fmul	d [ScaleZ]
	fstp	d [ebx.z3d]

	add	ebx, size point3d
	loop	SS3d_scale
	ret

;------------------------------------------------------------

.data

str3d_ox	dq ?
str3d_oy	dq ?
str3d_oz	dq ?

ScaleX		dd LETGEN_SCALEX
ScaleY		dd LETGEN_SCALEY
ScaleZ		dd LETGEN_SCALEZ
