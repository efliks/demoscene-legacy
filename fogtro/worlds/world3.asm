
World3_GetMem:
	mov	eax, W3_LETTER
	call	GetLetterParams
	mov	d [Cube3_Strc + S3D_N_VERTS], eax
	mov	d [Cube3_Strc + S3D_N_FACES], ebx

	add	d [World3_Strc + S3D_N_VERTS], eax
	add	d [World3_Strc + S3D_N_FACES], ebx

	mov	esi, o World3_Strc
	call	S3d_ComputeMem
	ret

;************************************************************

World3_Init:
	mov	esi, o World3_Strc
	call	S3d_AdjustMem
	push	eax

	mov	esi, o World3_Strc
	mov	edi, o Floor3_Strc
	xor	eax, eax
	xor	ebx, ebx
	call	S3d_SetPointers

	mov	esi, o World3_Strc
	mov	edi, o Cube3_Strc
	call	S3d_SetPointers

	mov	esi, o World3_Strc
	mov	edi, o Cube31_Strc
	call	S3d_SetPointers

	mov	esi, o World3_Strc
	mov	edi, o Cube32_Strc
	call	S3d_SetPointers

	mov	esi, o World3_Strc
	mov	edi, o Cube33_Strc
	call	S3d_SetPointers

	mov	esi, o Floor3_Strc
	mov	eax, FLOOR3_SIDE
	mov	ebx, FLOOR3_CELL
	call	S3d_GenerateFloor

	mov	esi, o Cube3_Strc
	mov	eax, W3_LETTER
	mov	ebx, 250
	call	MakeLetter3d

	mov	esi, o Cube31_Strc
	mov	eax, 25
	call	S3d_GenerateCube

	mov	esi, o Cube32_Strc
	mov	eax, 35
	call	S3d_GenerateCube

	mov	esi, o Cube33_Strc
	mov	eax, 45
	call	S3d_GenerateCube

	mov	esi, o Cube3_Strc
	mov	eax, FLOOR3_N_VERTS
	call	S3d_CorrectFaces

	mov	eax, FLOOR3_N_VERTS
	add	eax, d [Cube3_Strc + S3D_N_VERTS]

	mov	esi, o Cube31_Strc
	push	eax
	call	S3d_CorrectFaces

	mov	esi, o Cube32_Strc
	pop	eax
	add	eax, 8
	push	eax
	call	S3d_CorrectFaces

	mov	esi, o Cube33_Strc
	pop	eax
	add	eax, 8
	call	S3d_CorrectFaces


	mov	edi, d [Floor3_Strc + S3D_PTR_B_VERTS]
	xor	esi, esi
w3i_1:
	xor	ecx, ecx
w3i_2:
	mov	eax, ecx
	sub	eax, FLOOR3_SIDE / 2
	mov	ebx, FLOOR3_CELL
	imul	ebx
	mov	d [_w3temp], eax
	fild	d [_w3temp]
	fmul	st(0), st(0)

	mov	eax, esi
	sub	eax, FLOOR3_SIDE / 2
	imul	ebx
	mov	d [_w3temp], eax
	fild	d [_w3temp]
	fmul	st(0), st(0)

	faddp	st(1), st(0)
	fsqrt
	fmul	[_w3correct]
	fadd	[_w3height]
	fstp	d [edi + 4]
	

	add	edi, 12
	inc	ecx
	cmp	ecx, FLOOR3_SIDE
	jne	w3i_2
	inc	esi
	cmp	esi, FLOOR3_SIDE
	jne	w3i_1

	
	pop	eax
	ret

;************************************************************

World3_Proc:
	mov	esi, o World3_World
	call	RenderWorld

	

	fld	d [Cube3_ang]
	fadd	d [Cube3_ang_d]
	fst	d [Cube3_ang]
	fsin
	fmul	d [Cube3_A]
	fadd	d [Cube3_MinH]
	fstp	d [Cube3_Obj + O_POS + 4]

	; move 1st cube
	fld	d [SCube_ang]
	fadd	d [SCube_ang_d]
	fst	d [SCube_ang]
	fld	st(0)
	fsin
	fmul	d [SCube_r]
	fstp	d [Cube31_Obj + O_POS + 8]
	fcos
	fmul	d [SCube_r]
	fstp	d [Cube31_Obj + O_POS + 0]

	; move 2nd cube
	fld	d [SCube_ang]
	fsub	d [SCube_ang_sub]
	fld	st(0)
	fsin
	fmul	d [SCube_r]
	fstp	d [Cube32_Obj + O_POS + 8]
	fcos
	fmul	d [SCube_r]
	fstp	d [Cube32_Obj + O_POS + 0]

	; move 3rd cube
	fld	d [SCube_ang]
	fsub	d [SCube_ang_sub]
	fsub	d [SCube_ang_sub]
	fld	st(0)
	fsin
	fmul	d [SCube_r]
	fstp	d [Cube33_Obj + O_POS + 8]
	fcos
	fmul	d [SCube_r]
	fstp	d [Cube33_Obj + O_POS + 0]

	ret

;************************************************************

Cube3_ang	dd 0
Cube3_ang_d	dd 0.008
Cube3_A		dd 200.0
Cube3_MinH	dd 210.0

SCube_ang	dd 0
SCube_ang_d	dd 0.008
SCube_r		dd 350.0
SCube_ang_sub	dd 0.800

_w3temp		dd 0
_w3correct	dd 0.25
_w3height	dd -130.0

