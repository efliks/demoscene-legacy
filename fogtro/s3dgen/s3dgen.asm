
;************************************************************
;	S3d_CorrectFaces()
;
;	in: esi = ptr to struct3d, eax = starting vertex number
;************************************************************
S3d_CorrectFaces:
	mov	edi, d [esi + S3D_PTR_FACES]
	mov	ecx, d [esi + S3D_N_FACES]
SCF_do:
	add	w [edi + 0], ax
	add	w [edi + 2], ax
	add	w [edi + 4], ax
	add	edi, 6
	dec	ecx
	jnz	SCF_do
	ret

;************************************************************
;	S3d_SetPointers()
;
;	in:	esi = ptr to big struct, edi = ptr to small struct,
;		eax = starting number of verts,
;		ebx = starting number of faces
;	out:	eax = modified number of verts,
;		ebx = mofified number of faces 
;************************************************************
S3d_SetPointers:
	mov	d [_nv], eax
	mov	d [_nf], ebx

	mov	edx, eax
	shl	eax, 2
	shl	edx, 3
	add	edx, eax

	mov	eax, d [esi + S3D_PTR_B_VERTS]
	add	eax, edx
	mov	d [edi + S3D_PTR_B_VERTS], eax

	mov	eax, d [esi + S3D_PTR_R_VERTS]
	add	eax, edx
	mov	d [edi + S3D_PTR_R_VERTS], eax

	mov	edx, d [_nv]
	shl	edx, 2

	mov	eax, d [esi + S3D_PTR_T_VERTS]
	add	eax, edx
	mov	d [edi + S3D_PTR_T_VERTS], eax

	mov	eax, ebx
	shl	eax, 1
	shl	ebx, 2
	add	ebx, eax

	mov	eax, d [esi + S3D_PTR_FACES]
	add	eax, ebx
	mov	d [edi + S3D_PTR_FACES], eax

	mov	edx, d [_nf]
	shl	edx, 1

	mov	eax, d [esi + S3D_PTR_DEPTH]
	add	eax, edx
	mov	d [edi + S3D_PTR_DEPTH], eax

	mov	eax, d [esi + S3D_PTR_ORDER]
	add	eax, edx
	mov	d [edi + S3D_PTR_ORDER], eax


	mov	eax, d [edi + S3D_N_VERTS]
	add	eax, d [_nv]
	mov	ebx, d [edi + S3D_N_FACES]
	add	ebx, d [_nf]
	ret


_nv	dd 0
_nf	dd 0

;************************************************************
;	S3d_SumNums()
;
;	in: esi = ptr to table of structs, ecx = number of
;	structs, edi = ptr to main struct
;************************************************************
S3d_SumNums:
	xor	eax, eax
	xor	ebx, ebx
SSN_add:
	add	eax, d [esi + S3D_N_VERTS]
	add	ebx, d [esi + S3D_N_FACES]
	add	esi, STRUCT3D_SIZE
	dec	ecx
	jnz	SSN_add

	mov	d [edi + S3D_N_VERTS], eax
	mov	d [edi + S3D_N_FACES], ebx
	ret

;************************************************************
;	S3d_ComputeMem()
;
;	in: esi = ptr to struct3d
;	out: eax = requested memory
;************************************************************
S3d_ComputeMem:
	mov	ecx, d [esi + S3D_N_VERTS]
	mov	edx, d [esi + S3D_N_FACES]

	mov	eax, ecx

	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	shl	eax, 1
	push	eax

	mov	eax, edx

	mov	ebx, eax
	shl	eax, 1
	shl	ebx, 2
	add	eax, ebx
	pop	ebx
	add	eax, ebx
	push	eax

	mov	eax, ecx
	shl	eax, 2
	pop	ebx
	add	eax, ebx
	push	eax

	mov	eax, edx

	shl	eax, 2
	pop	ebx
	add	eax, ebx
	ret

;************************************************************
;	S3d_AdjustMem()
;
;	in: esi = ptr to struct3d,
;	eax = ptr to allocated memory block
;	out: eax = modified pointer
;************************************************************
S3d_AdjustMem:
	mov	ebx, d [esi + S3D_N_VERTS]
	push	ebx
	mov	ecx, ebx
	shl	ebx, 2
	shl	ecx, 3
	add	ebx, ecx	; ebx = n_verts * 12

	pop	ecx
	shl	ecx, 2		; ecx = n_verts * 4

	mov	edx, d [esi + S3D_N_FACES]
	push	edx
	mov	ebp, edx
	shl	edx, 1
	shl	ebp, 2
	add	edx, ebp	; edx = n_faces * 6

	pop	ebp
	shl	ebp, 1		; ebp = n_faces * 2

	mov	d [esi + S3D_PTR_B_VERTS], eax
	add	eax, ebx
	mov	d [esi + S3D_PTR_R_VERTS], eax
	add	eax, ebx
	mov	d [esi + S3D_PTR_T_VERTS], eax
	add	eax, ecx
	mov	d [esi + S3D_PTR_FACES], eax
	add	eax, edx
	mov	d [esi + S3D_PTR_DEPTH], eax
	add	eax, ebp
	mov	d [esi + S3D_PTR_ORDER], eax
	add	eax, ebp
	ret

;************************************************************
;	S3d_Long2Float()
;
;	in: esi = ptr to struct3d
;************************************************************
S3d_Long2Float:
	mov	edi, d [esi + S3D_PTR_B_VERTS]
	mov	ecx, d [esi + S3D_N_VERTS]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
SL2F_loop:
	fild	d [edi]
	fstp	d [edi]
	add	edi, 4
	dec	ecx
	jnz	SL2F_loop
	ret

;************************************************************
;	S3d_GenerateFloor()
;
;	in: esi = ptr to struct3d, eax = floor side,
;	ebx = floor cell
;************************************************************
S3d_GenerateFloor:
	mov	edi, d [esi + S3D_PTR_B_VERTS]
	push	eax
	push	esi
	mov	esi, eax	; esi = floor side

	cld
	xor	ebp, ebp
SGF_y:
	xor	ecx, ecx
SGF_x:
	mov	eax, ecx
	imul	ebx
	stosd
	xor	eax, eax
	stosd
	mov	eax, ebp
	neg	eax
	add	eax, esi
	imul	ebx
	stosd
	inc	ecx
	cmp	ecx, esi
	jne	SGF_x
	inc	ebp
	cmp	ebp, esi
	jne	SGF_y

	pop	esi
	call	S3d_Long2Float

	mov	edi, esi
	call	CenterStruct3dA


	mov	esi, edi
	pop	ebp		; ebp = floor side
	mov	edi, d [esi + S3D_PTR_FACES]
	xor	ebx, ebx
SGF_yy:
	xor	ecx, ecx
SGF_xx:
	mov	eax, ebx
	mul	ebp
	add	eax, ecx
	push	eax
	stosw
	inc	eax
	stosw
	add	eax, ebp
	stosw
	pop	eax
	stosw
	inc	eax
	add	eax, ebp
	stosw
	dec	eax
	stosw
	inc	ecx
	mov	eax, ebp
	dec	eax
	cmp	ecx, eax
	jne	SGF_xx
	inc	ebx
	mov	eax, ebp
	dec	eax
	cmp	ebx, eax
	jne	SGF_yy
	ret

;************************************************************
;	S3d_GenerateCube()
;
;	in: esi = ptr to struct3d, eax = size
;************************************************************
S3d_GenerateCube:
	mov	ebp, esi

	mov	ebx, eax
	mov	esi, o _CubeVerts
	mov	edi, d [ebp + S3D_PTR_B_VERTS]
	mov	ecx, 8 * 3
SGC_verts:
	lodsb
	movsx	eax, al
	imul	ebx
	stosd
	dec	ecx
	jnz	SGC_verts

	mov	esi, ebp
	call	S3d_Long2Float

	mov	esi, o _CubeFaces
	mov	edi, d [ebp + S3D_PTR_FACES]
	mov	ecx, 12 * 3
	cld
	rep	movsw
	ret

;************************************************************

_CubeVerts	db -1, -1,  1,   1, -1,  1
		db  1,  1,  1,  -1,  1,  1
		db -1, -1, -1,   1, -1, -1
		db  1,  1, -1,  -1,  1, -1

_CubeFaces	dw 0, 1, 2,  0, 2, 3
		dw 5, 4, 7,  5, 7, 6
		dw 1, 5, 6,  1, 6, 2
		dw 4, 0, 3,  4, 3, 7
		dw 4, 5, 1,  1, 0, 4
		dw 3, 2, 6,  3, 6, 7
