
; in: esi = ptr to struct3d
InitMovement:
	mov	ebp, esi

	mov	esi, o StartX
	mov	edi, o DestX
	call	GetVectorLenght
	fstp	d [ebp.s3d_MovingTime]
	mov	d [ebp.s3d_CurrTime], 0
	mov	d [ebp.s3d_Movement], 1

	fld	d [DestX]
	fsub	d [StartX]
	fdiv	d [ebp.s3d_MovingTime]
	fstp	d [ebp.s3d_DeltaX]

	fld	d [DestY]
	fsub	d [StartY]
	fdiv	d [ebp.s3d_MovingTime]
	fstp	d [ebp.s3d_DeltaY]

	fld	d [DestZ]
	fsub	d [StartZ]
	fdiv	d [ebp.s3d_MovingTime]
	fstp	d [ebp.s3d_DeltaZ]
	ret

;------------------------------------------------------------

; in: esi = ptr to struct3d
MoveStruct3d:
	push	esi
	lea	esi, d [esi.s3d_AngleX]
	mov	edi, o rot_matrix
	call	mx_rotation_matrix
	pop	ebp

	mov	esi, d [ebp.s3d_points]
	mov	edi, d [ebp.s3d_r_points]
	mov	ecx, d [ebp.s3d_n_points]
	mov	ebx, o rot_matrix
	call	mx_rotate_points

	mov	esi, d [ebp.s3d_point_nrm]
	mov	edi, d [ebp.s3d_r_point_nrm]
	mov	ecx, d [ebp.s3d_n_points]
	call	mx_rotate_points


	cmp	d [ebp.s3d_Movement], 1
	jne	MS3d_MV

	fld	d [ebp.s3d_CurrTime]
	fadd	d [MovingSpeed]
	fst	d [ebp.s3d_CurrTime]
	fcomp	d [ebp.s3d_MovingTime]
	fstsw	ax
	sahf
	ja	MS3d_DisableMovement
	
	fld	d [ebp.s3d_DeltaX]
	fmul	d [ebp.s3d_CurrTime]
	fadd	d [StartX]
	fstp	d [AddX]

	fld	d [ebp.s3d_DeltaY]
	fmul	d [ebp.s3d_CurrTime]
	fadd	d [StartY]
	fstp	d [AddY]

	fld	d [ebp.s3d_DeltaZ]
	fmul	d [ebp.s3d_CurrTime]
	fadd	d [StartZ]
	fstp	d [AddZ]

	jmp	MS3d_MV
MS3d_DisableMovement:
	mov	d [ebp.s3d_Movement], 0

MS3d_MV:
	mov	edi, d [ebp.s3d_r_points]
	mov	ecx, d [ebp.s3d_n_points]
MS3d_MoveVertices:
	fld	d [edi.x3d]
	fadd	d [AddX]
	fstp	d [edi.x3d]

	fld	d [edi.y3d]
	fadd	d [AddY]
	fstp	d [edi.y3d]

	fld	d [edi.z3d]
	fadd	d [AddZ]
	fstp	d [edi.z3d]

	add	edi, size point3d
	dec	ecx
	jnz	MS3d_MoveVertices


	mov	esi, d [ebp.s3d_r_points]
	mov	edi, d [ebp.s3d_t_points]
	mov	ecx, d [ebp.s3d_n_points]
	call	translate_points
	ret

;------------------------------------------------------------

MovingSpeed	dd M_SPEED

AddX		dd ?
AddY		dd ?
AddZ		dd ?


StartX		dd ?
StartY		dd ?
StartZ		dd ?

DestX		dd ?
DestY		dd ?
DestZ		dd ?
