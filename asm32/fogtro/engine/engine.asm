
;************************************************************
;	MakeVector()
;
;	in: esi = ptr to 1st vertex, edi = ptr to 2nd vertex,
;	ebx = ptr to result vector
;************************************************************
MakeVector:
	fld	d [edi + 0]
	fsub	d [esi + 0]

	fld	d [edi + 4]
	fsub	d [esi + 4]

	fld	d [edi + 8]
	fsub	d [esi + 8]

	fstp	d [ebx + 8]
	fstp	d [ebx + 4]
	fstp	d [ebx + 0]
	ret

;************************************************************
;	GetVectorLenght()
;
;	in: esi = ptr to 1st vertex, edi = ptr to 2nd vertex
;	out: st(0) = lenght
;************************************************************
GetVectorLenght:
	fld	d [edi + 0]
	fsub	d [esi + 0]
	fmul	st(0), st(0)

	fld	d [edi + 4]
	fsub	d [esi + 4]
	fmul	st(0), st(0)

	fld	d [edi + 8]
	fsub	d [esi + 8]
	fmul	st(0), st(0)

	faddp	st(1), st(0)
	faddp	st(1), st(0)
	fsqrt
	ret

;************************************************************
;	NormalizeVector()
;
;	in: edi = ptr to vector
;************************************************************
NormalizeVector:
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

;************************************************************
;	CrossProduct()
;
;	in: esi = ptr to 1st vector, edi = ptr to 2nd vector,
;	ebx = ptr to result vector
;************************************************************
CrossProduct:
	fld	d [esi + 4]
	fmul	d [edi + 8]
	fld	d [esi + 8]
	fmul	d [edi + 4]
	fsubp	st(1), st(0)
	fstp	d [ebx + 0]

	fld	d [esi + 8]
	fmul	d [edi + 0]
	fld	d [esi + 0]
	fmul	d [edi + 8]
	fsubp	st(1), st(0)
	fstp	d [ebx + 4]

	fld	d [esi + 0]
	fmul	d [edi + 4]
	fld	d [esi + 4]
	fmul	d [edi + 0]
	fsubp	st(1), st(0)
	fstp	d [ebx + 8]
	ret

;************************************************************
;	DotProduct()
;
;	in: esi = ptr to 1st vector, edi = ptr to 2nd vector
;	out: st(0) = dot-product
;************************************************************
DotProduct:
	fld	d [esi + 0]
	fmul	d [edi + 0]

	fld	d [esi + 4]
	fmul	d [edi + 4]

	fld	d [esi + 8]
	fmul	d [edi + 8]

	faddp	st(1), st(0)
	faddp	st(1), st(0)
	ret

;************************************************************
;	MakeRotationMatrix()
;
;	in: esi = ptr to angles (float), edi = ptr to matrix
;************************************************************
MakeRotationMatrix:
	fld	d [esi + 0]
	fld	st(0)
	fsin
	fstp	d [sin_x]
	fcos
	fstp	d [cos_x]

	fld	d [esi + 4]
	fld	st(0)
	fsin
	fstp	d [sin_y]
	fcos
	fstp	d [cos_y]

	fld	d [esi + 8]
	fld	st(0)
	fsin
	fstp	d [sin_z]
	fcos
	fstp	d [cos_z]


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
	ret

;************************************************************
;	TransformVector()
;
;	in: edi = ptr to vector, ebx = ptr to matrix
;************************************************************
TransformVector:
	mov	eax, esp
	push	d [edi + 0]
	push	d [edi + 4]
	push	d [edi + 8]

	fld	d [eax - 4]
	fmul	d [ebx + 0 * 4 + 0 * 12]
	fld	d [eax - 8]
	fmul	d [ebx + 0 * 4 + 1 * 12]
	faddp	st(1), st(0)
	fld	d [eax - 12]
	fmul	d [ebx + 0 * 4 + 2 * 12]
	faddp	st(1), st(0)
	fstp	d [edi + 0]

	fld	d [eax - 4]
	fmul	d [ebx + 1 * 4 + 0 * 12]
	fld	d [eax - 8]
	fmul	d [ebx + 1 * 4 + 1 * 12]
	faddp	st(1), st(0)
	fld	d [eax - 12]
	fmul	d [ebx + 1 * 4 + 2 * 12]
	faddp	st(1), st(0)
	fstp	d [edi + 4]

	fld	d [eax - 4]
	fmul	d [ebx + 2 * 4 + 0 * 12]
	fld	d [eax - 8]
	fmul	d [ebx + 2 * 4 + 1 * 12]
	faddp	st(1), st(0)
	fld	d [eax - 12]
	fmul	d [ebx + 2 * 4 + 2 * 12]
	faddp	st(1), st(0)
	fstp	d [edi + 8]

	add	esp, 12
	ret

;************************************************************
;	TranslateVertices()
;
;	in: esi = ptr to 3d vertices, edi = ptr to 2d vertices,
;	ecx = number of vetices
;************************************************************
TranslateVertices:
	mov	d [SkippedVerts], 0
TV_do:
	fld	d [esi + 8]
	ftst
	fstsw	ax
	sahf
	jae	TV_ok
	ffree	st(0)
	mov	w [esi + 0], -1
	inc	d [SkippedVerts]
	jmp	TV_next
TV_ok:
	fld	d [esi + 0]
	fmul	d [TV_persp]
	fdiv	st(0), st(1)
	fadd	d [TV_corrx]
	fistp	w [edi + 0]

	fld	d [esi + 4]
	fmul	d [TV_persp]
	fdivrp	st(1), st(0)
	fadd	d [TV_corry]
	fistp	w [edi + 2]
TV_next:
	add	esi, 12
	add	edi, 4
	dec	ecx
	jnz	TV_do
	ret

;************************************************************
;	MakeCameraMatrix()
;
;	in: edi = ptr to camera
;************************************************************
MakeCameraMatrix:
	mov	ebp, edi

	fld	d [ebp + CAM_ROLL]
	fld	st(0)
	fsin
	fstp	d [cam_vec_u + 0]
	fcos
	fchs
	fstp	d [cam_vec_u + 4]
	xor	eax, eax
	mov	d [cam_vec_u + 8], eax

	lea	esi, [ebp + CAM_POS]
	lea	edi, [ebp + CAM_TARGET]
	mov	ebx, o cam_vec_f
	call	MakeVector
	mov	edi, ebx
	call	NormalizeVector

	mov	esi, o cam_vec_u
	mov	edi, o cam_vec_f
	mov	ebx, o cam_vec_r
	call	CrossProduct
	mov	edi, ebx
	call	NormalizeVector

	mov	esi, o cam_vec_f
	mov	edi, o cam_vec_r
	mov	ebx, o cam_vec_u
	call	CrossProduct

	mov	eax, d [cam_vec_r + 0]
	mov	d [ebp + CAM_MATRIX + 0 * 4 + 0 * 12], eax
	mov	eax, d [cam_vec_r + 4]
	mov	d [ebp + CAM_MATRIX + 0 * 4 + 1 * 12], eax
	mov	eax, d [cam_vec_r + 8]
	mov	d [ebp + CAM_MATRIX + 0 * 4 + 2 * 12], eax

	mov	eax, d [cam_vec_u + 0]
	mov	d [ebp + CAM_MATRIX + 1 * 4 + 0 * 12], eax
	mov	eax, d [cam_vec_u + 4]
	mov	d [ebp + CAM_MATRIX + 1 * 4 + 1 * 12], eax
	mov	eax, d [cam_vec_u + 8]
	mov	d [ebp + CAM_MATRIX + 1 * 4 + 2 * 12], eax

	mov	eax, d [cam_vec_f + 0]
	mov	d [ebp + CAM_MATRIX + 2 * 4 + 0 * 12], eax
	mov	eax, d [cam_vec_f + 4]
	mov	d [ebp + CAM_MATRIX + 2 * 4 + 1 * 12], eax
	mov	eax, d [cam_vec_f + 8]
	mov	d [ebp + CAM_MATRIX + 2 * 4 + 2 * 12], eax
	ret

;************************************************************
;	ObjectProc()
;
;	in: esi = ptr to object3d
;************************************************************
ObjectProc:
	cmp	d [esi + O_FLAGS], OF_STATIC
	jne	OP_ok
	ret
OP_ok:
	push	ebp
	push	ecx
	push	esi

	mov	ebp, esi
	mov	esi, d [ebp + O_PTR_STRUCT3D]

	lea	edi, [ebp + O_MATRIX]
	mov	ebx, edi
	push	esi
	lea	esi, [ebp + O_ANGLE_X]
	call	MakeRotationMatrix
	pop	esi

	mov	edi, d [esi + S3D_PTR_R_VERTS]
	mov	ecx, d [esi + S3D_N_VERTS]
OP_r:
	call	TransformVector
	fld	d [edi + 0]
	fadd	d [ebp + O_POS + 0]
	fstp	d [edi + 0]
	fld	d [edi + 4]
	fadd	d [ebp + O_POS + 4]
	fstp	d [edi + 4]
	fld	d [edi + 8]
	fadd	d [ebp + O_POS + 8]
	fstp	d [edi + 8]
	add	edi, 12
	dec	ecx
	jnz	OP_r

	fld	d [ebp + O_ANGLE_X]
	fadd	d [ebp + O_ADD_ANGLE_X]
	fstp	d [ebp + O_ANGLE_X]
	fld	d [ebp + O_ANGLE_Y]
	fadd	d [ebp + O_ADD_ANGLE_Y]
	fstp	d [ebp + O_ANGLE_Y]
	fld	d [ebp + O_ANGLE_Z]
	fadd	d [ebp + O_ADD_ANGLE_Z]
	fstp	d [ebp + O_ANGLE_Z]

	pop	esi
	pop	ecx
	pop	ebp
	ret

;************************************************************
;	RenderWorld()
;
;	in: esi = ptr to world
;************************************************************
RenderWorld:
	mov	ebp, esi

	;mov	eax, d [ebp + W_FLAGS]
	;mov	d [CurrLightModel], eax

	mov	edx, d [ebp + W_PTR_STRUCT3D]
	mov	ecx, d [edx + S3D_N_VERTS]
	push	edx
	push	ecx

	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	mov	esi, d [edx + S3D_PTR_B_VERTS]
	mov	edi, d [edx + S3D_PTR_R_VERTS]
	push	edi
	cld
	rep	movsd

	mov	ecx, d [ebp + W_NUM_OF_OBJECTS]
	mov	esi, d [ebp + W_PTR_OBJECTS]
RW_ObjLoop:
	call	ObjectProc
	add	esi, OBJECT3D_SIZE
	dec	ecx
	jnz	RW_ObjLoop

	push	ebp
	mov	edi, d [ebp + W_PTR_CAMERA]
	mov	d [CurrCamPtr], edi ; <- requied by GetVertexColorDist()
	push	edi
	push	edi
	call	UpdateCamera
	pop	edi
	call	MakeCameraMatrix
	pop	esi
	lea	ebx, [esi + CAM_MATRIX]
	pop	ebp

	pop	edi
	pop	ecx
	pop	edx
	push	ecx
	push	edi
RW_CamLoop:
	fld	d [edi + 0]
	fsub	d [esi + CAM_POS + 0]
	fstp	d [edi + 0]
	fld	d [edi + 4]
	fsub	d [esi + CAM_POS + 4]
	fstp	d [edi + 4]
	fld	d [edi + 8]
	fsub	d [esi + CAM_POS + 8]
	fstp	d [edi + 8]
	call	TransformVector
	add	edi, 12
	dec	ecx
	jnz	RW_CamLoop

	pop	esi
	pop	ecx
	mov	edi, d [edx + S3D_PTR_T_VERTS]
	call	TranslateVertices

	push	edx
	mov	esi, edx
	call	sort_faces
	pop	edx
	call	render_struct3d
	ret

;************************************************************
;	NormalizeStruct3d()
;
;	in: esi = ptr to struct3d
;************************************************************
NormalizeStruct3d:
	call	GetStruct3dRadius
	
	mov	edi, d [esi + S3D_PTR_B_VERTS]
	mov	ecx, d [esi + S3D_N_VERTS]
NS3d_do:
	fld	d [edi + 0]
	fdiv	st(0), st(1)
	fstp	d [edi + 0]
	fld	d [edi + 4]
	fdiv	st(0), st(1)
	fstp	d [edi + 4]
	fld	d [edi + 8]
	fdiv	st(0), st(1)
	fstp	d [edi + 8]
	add	edi, 12
	dec	ecx
	jnz	NS3d_do

	ffree	st(0)
	ret

;************************************************************
;	GetStruct3dRadius()
;
;	in: esi = ptr to struct3d
;	out: st(0) = radius
;************************************************************
GetStruct3dRadius:
	mov	edi, d [esi + S3D_PTR_B_VERTS]
	mov	ecx, d [esi + S3D_N_VERTS]

	xor	eax, eax
	mov	ebx, esp
	push	eax
GSR_get:
	fld	d [edi + 0]
	fmul	st(0), st(0)
	fld	d [edi + 4]
	fmul	st(0), st(0)
	faddp	st(1), st(0)
	fld	d [edi + 8]
	fmul	st(0), st(0)
	faddp	st(1), st(0)
	fcom	d [ebx - 4]
	fstsw	ax
	sahf
	jb	GSR_next
	fst	d [ebx - 4]
GSR_next:
	ffree	st(0)
	add	edi, 12
	dec	ecx
	jnz	GSR_get

	fld	d [ebx - 4]
	fsqrt
	pop	eax
	ret

;************************************************************
;	ScaleStruct3d()
;
;	in: edi = ptr to struct3d,
;	st(0) = scalez, st(1) = scaley, st(2) = scalex
;************************************************************
ScaleStruct3d:
	mov	ecx, d [edi + S3D_N_VERTS]
	mov	edi, d [edi + S3D_PTR_B_VERTS]
SS3d_loop:
	fld	d [edi + 0]
	fmul	st(0), st(3)
	fstp	d [edi + 0]

	fld	d [edi + 4]
	fmul	st(0), st(2)
	fstp	d [edi + 4]

	fld	d [edi + 8]
	fmul	st(0), st(1)
	fstp	d [edi + 8]

	add	edi, 12
	dec	ecx
	jnz	SS3d_loop

	ffree	st(0)
	ffree	st(0)
	ffree	st(0)
	ret

;************************************************************
;	CenterStruct3dA()
;
;	in: edi = ptr to struct3d
;************************************************************
CenterStruct3dA:
	push	edi
	mov	edi, o min_x
	xor	eax, eax
	mov	ecx, 6
	rep	stosd
	pop	edi
	
	mov	esi, d [edi + S3D_PTR_B_VERTS]
	mov	ecx, d [edi + S3D_N_VERTS]
CS3dA_1:
	push	ecx
	

	mov	ecx, 3
	mov	ebp, o min_x
CS3dA_2:
	fld	d [esi]
	fcom	d [ebp + 0] ; min
	fstsw	ax
	sahf
	ja	CS3dA_skip1
	fstp	d [ebp + 0]
	jmp	CS3dA_ok1
CS3dA_skip1:
	ffree	st(0)
CS3dA_ok1:
	fld	d [esi]
	fcom	d [ebp + 4] ; max
	fstsw	ax
	sahf
	jb	CS3dA_skip2
	fstp	d [ebp + 4]
	jmp	CS3dA_ok2
CS3dA_skip2:
	ffree	st(0)
CS3dA_ok2:
	add	esi, 4
	add	ebp, 8
	loop	CS3dA_2

	pop	ecx
	loop	CS3dA_1


	mov	ecx, 3
	mov	ebp, o min_x
CS3dA_make:
	fld	d [ebp + 4] ; max
	fsub	d [ebp + 0] ; min
	fmul	d [CS3dA_mulval]
	fstp	d [ebp + 0] ; min
	add	ebp, 8
	loop	CS3dA_make

	mov	esi, d [edi + S3D_PTR_B_VERTS]
	mov	ecx, d [edi + S3D_N_VERTS]
CS3dA_final:
	fld	d [esi + 0]
	fsub	d [min_x]
	fstp	d [esi + 0]

	fld	d [esi + 4]
	fsub	d [min_y]
	fstp	d [esi + 4]

	fld	d [esi + 8]
	fsub	d [min_z]
	fstp	d [esi + 8]

	add	esi, 12
	loop	CS3dA_final
	ret

;************************************************************
;	MakeLinearTrack()
;
;	in: esi = source vertex, edi = destination vertex,
;	ebx = ptr to track, ecx = track lenght,
;	eax = ptr to allocated memory block
;************************************************************
MakeLinearTrack:
	mov	ebp, ebx
	mov	d [ebp + T_NUM_OF_VERTS], ecx
	mov	d [ebp + T_PTR_VERTS], eax
	mov	ebp, eax

	sub	esp, 12 + 4
	mov	ebx, esp
	call	MakeVector

	mov	d [ebx + 12], ecx
	fild	d [ebx + 12]

	fld	d [ebx + 0]
	fdiv	st(0), st(1)
	fstp	d [ebx + 0]

	fld	d [ebx + 4]
	fdiv	st(0), st(1)
	fstp	d [ebx + 4]

	fld	d [ebx + 8]
	fdivrp	st(1), st(0)
	fstp	d [ebx + 8]

	fld	d [esi + 0]
	fld	d [esi + 4]
	fld	d [esi + 8]

	;	st0	st1	st2	st3	st4	st5	st6	st7
	;	z	y	x

MLT_do:
	fxch	st(2)
	fst	d [ebp + 0]
	fadd	d [ebx + 0]
	fxch	st(2)

	fxch	st(1)
	fst	d [ebp + 4]
	fadd	d [ebx + 4]
	fxch	st(1)

	fst	d [ebp + 8]
	fadd	d [ebx + 8]
	
	add	ebp, 12
	dec	ecx
	jnz	MLT_do

	;ffree	st(0)
	;ffree	st(0)
	;ffree	st(0)
	finit

	add	esp, 12 + 4
	ret

;************************************************************
;	GetVertexColorDist()
;
;	in: esi = ptr to struct3d, eax = number of vertex (1, 2 or 3),
;	ebx = number of face,
;	out: eax = color
;
;	Funcion uses CurrCamPtr as a pointer to camera !!!
;************************************************************
GetVertexColorDist:
	mov	ecx, ebx
	shl	ecx, 1
	shl	ebx, 2
	add	ebx, ecx
	add	ebx, d [esi + S3D_PTR_FACES]
	dec	eax
	shl	eax, 1
	add	ebx, eax

	movzx	eax, w [ebx]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	ebx, eax
	;add	ebx, d [esi + S3D_PTR_B_VERTS]
	add	ebx, d [esi + S3D_PTR_R_VERTS]

	push	edi
	push	esi
	;mov	esi, d [CurrCamPtr]
	;lea	esi, [esi + CAM_POS]
	mov	esi, o DefaultCamPos
	mov	edi, ebx
	call	GetVectorLenght
	fmul	d [GVCD_z_const]
	pop	esi
	pop	edi

	mov	ebx, esp
	push	eax
	fistp	d [ebx - 4]
	mov	eax, d [ebx - 4]
	pop	ebx
	neg	eax
	add	eax, 63

	add	eax, MOVE_PAL_COLOR

	or	eax, eax
	jg	GVCD_ok1
	mov	eax, MIN_COLOR
	jmp	GVCD_ok2
GVCD_ok1:
	cmp	eax, 63
	jle	GVCD_ok2
	mov	eax, 63
GVCD_ok2:
	ret

;************************************************************
;	GetVertexColorZ()
;
;	in: esi = ptr to struct3d, eax = number of vertex (1, 2 or 3),
;	ebx = number of face
;	out: eax = color
;************************************************************
GetVertexColorZ:
	mov	ecx, ebx
	shl	ecx, 1
	shl	ebx, 2
	add	ebx, ecx
	add	ebx, d [esi + S3D_PTR_FACES]
	dec	eax
	shl	eax, 1
	add	ebx, eax

	movzx	eax, w [ebx]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	ebx, eax
	add	ebx, d [esi + S3D_PTR_R_VERTS]

	fld	d [ebx + 8]
	fmul	d [GVC_z_const]
	fsubr	d [GVC_n_colors]
	mov	ebx, esp
	push	eax
	fistp	d [ebx - 4]
	mov	eax, d [ebx - 4]
	pop	ebx

	add	eax, MOVE_PAL_COLOR

	or	eax, eax
	jg	GVC_col_ok1
	mov	eax, MIN_COLOR
	jmp	GVC_col_ok2
GVC_col_ok1:
	cmp	eax, 63
	jle	GVC_col_ok2
	mov	eax, 63
GVC_col_ok2:
	ret

;************************************************************
;	UpdateCamera()
;
;	in: edi = ptr to camera
;************************************************************
UpdateCamera:
	mov	ebp, edi

	mov	eax, d [ebp + CAM_TRACK_POS]
	mov	ebx, d [ebp + CAM_TRACK_LEN]
	dec	ebx
	cmp	eax, ebx
	je	UP_reset1
	inc	eax
	jmp	UP_update1
UP_reset1:
	xor	eax, eax
UP_update1:
	mov	d [ebp + CAM_TRACK_POS], eax

	mov	ebx, d [ebp + CAM_FLAGS]
	cmp	ebx, CF_NO_TARGET
	je	UP_no_tar
	
	ret
UP_no_tar:
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	ebx, eax
	add	ebx, d [ebp + CAM_PTR_TRACK]
	mov	eax, d [ebx + 0]
	mov	d [ebp + CAM_POS + 0], eax
	mov	eax, d [ebx + 4]
	mov	d [ebp + CAM_POS + 4], eax
	mov	eax, d [ebx + 8]
	mov	d [ebp + CAM_POS + 8], eax
	xor	eax, eax
	mov	d [ebp + CAM_TARGET + 0], eax
	mov	d [ebp + CAM_TARGET + 4], eax
	mov	d [ebp + CAM_TARGET + 8], eax
	ret

;************************************************************
;	SetTrack()
;
;	in: esi = ptr to track, ecx = track lenght, edi = ptr
;	to camera, eax = track type
;************************************************************
SetTrack:
	mov	d [edi + CAM_PTR_TRACK], esi
	mov	d [edi + CAM_TRACK_LEN], ecx
	mov	d [edi + CAM_FLAGS], eax
	xor	eax, eax
	mov	d [edi + CAM_TRACK_POS], eax
	ret

;************************************************************
;	GetStruct3dSize()
;
;	in: esi = ptr to struct3d, eax = 'x', 'y' or 'z'
;	out: st(0) = size
;************************************************************
GetStruct3dSize:
	ret

;************************************************************

TV_persp	dd 256.0
TV_stdcx	dd 160.0
TV_stdcy	dd 100.0
TV_corrx	dd 160.0
TV_corry	dd 100.0

sin_x		dd 0
cos_x		dd 0
sin_y		dd 0
cos_y		dd 0
sin_z		dd 0
cos_z		dd 0

cam_vec_f	db 12 dup(0)
cam_vec_u	db 12 dup(0)
cam_vec_r	db 12 dup(0)

CS3dA_mulval	dd 0.5
min_x		dd 0
max_x		dd 0
min_y		dd 0
max_y		dd 0
min_z		dd 0
max_z		dd 0

SkippedVerts	dd 0

;CurrLightModel	dd 0

GVC_n_colors	dd 63.0
GVC_z_const	dd 0.04

GVCD_z_const	dd 0.04
CurrCamPtr	dd 0
DefaultCamPos	dd 0, 0, 0

CurrColorProc	dd o GetVertexColorDist
