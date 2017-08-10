	
World1_Proc:
	mov	esi, o World1_World
	call	RenderWorld
	ret

;************************************************************

; returns requied memory in EAX
World1_GetMem:
	mov	esi, o W1_text3d
	mov	edi, o LetTab1_Strc ; LetterTab1_Strc
	mov	ecx, W1_TEXT3D_LEN
	xor	eax, eax
	mov	d [_mem111], eax
	cld
W1GM_i:
	lodsb
	movzx	eax, al
	call	GetLetterParams
	add	d [World1_Strc + S3D_N_VERTS], eax
	add	d [World1_Strc + S3D_N_FACES], ebx
	mov	d [edi + S3D_N_VERTS], eax
	mov	d [edi + S3D_N_FACES], ebx
	push	esi
	push	edi
	push	ecx
	mov	esi, edi
	call	S3d_ComputeMem
	pop	ecx
	pop	edi
	pop	esi
	add	d [_mem111], eax
	add	edi, STRUCT3D_SIZE
	loop	W1GM_i

	mov	eax, d [Floor1_Strc + S3D_N_VERTS]
	add	d [World1_Strc + S3D_N_VERTS], eax
	mov	eax, d [Floor1_Strc + S3D_N_FACES]
	add	d [World1_Strc + S3D_N_FACES], eax

	mov	esi, o Floor1_Strc
	call	S3d_ComputeMem

	add	eax, d [_mem111]

comment #
	mov	eax, 'N'
	call	GetLetterParams
	mov	d [LetTab1_Strc + S3D_N_VERTS], eax
	mov	d [LetTab1_Strc + S3D_N_FACES], ebx

	mov	esi, o Floor1_Strc
	mov	ecx, 2
	mov	edi, o World1_Strc
	call	S3d_SumNums

	mov	esi, o Floor1_Strc
	call	S3d_ComputeMem
	push	eax

	mov	esi, o LetTab1_Strc
	call	S3d_ComputeMem
	pop	ebx
	add	eax, ebx #

	add	eax, (WORLD1_T_VERTS * T_STD_NUM_OF_VERTS * 12)
	ret

;************************************************************

; in: EAX = ptr to allocated memory block
; out: EAX = modified pointer
World1_Init:
	mov	esi, o World1_Strc
	call	S3d_AdjustMem

	mov	d [World1_Camera + CAM_PTR_TRACK], eax
	add	eax, (WORLD1_T_VERTS * T_STD_NUM_OF_VERTS * 12)
	push	eax

	mov	esi, o World1_Strc
	mov	edi, o Floor1_Strc
	xor	eax, eax
	xor	ebx, ebx
	call	S3d_SetPointers

	;mov	edi, o LetTab1_Strc
	;call	S3d_SetPointers

	mov	edi, o LetTab1_Strc
	mov	ecx, W1_TEXT3D_LEN
W1I_ptrs:
	push	edi
	push	ecx
	call	S3d_SetPointers
	pop	ecx
	pop	edi
	add	edi, STRUCT3D_SIZE
	loop	W1I_ptrs


	mov	esi, o Floor1_Strc
	mov	eax, FLOOR1_SIDE
	mov	ebx, FLOOR1_CELL
	call	S3d_GenerateFloor

	;mov	eax, 'N'
	;mov	ebx, 300
	;mov	esi, o LetTab1_Strc
	;call	LG_GenerateLetter

	mov	esi, o W1_text3d
	mov	edi, o LetTab1_Strc
	mov	ecx, W1_TEXT3D_LEN
	mov	ebp, FLOOR1_N_VERTS
	cld
W1I_gen:
	lodsb
	movzx	eax, al
	pushad
	pushad
	mov	ebx, 300
	mov	esi, edi
	call	LG_GenerateLetter
	popad
	mov	esi, edi
	mov	eax, ebp
	call	S3d_CorrectFaces
	popad
	add	ebp, d [edi + S3D_N_VERTS]
	add	edi, STRUCT3D_SIZE
	loop	W1I_gen

	;mov	esi, o LetTab1_Strc
	;mov	eax, FLOOR1_N_VERTS
	;call	S3d_CorrectFaces

	;mov	esi, o LetTab1_Strc
	;call	GetStruct3dRadius
	;fadd	d [MinObjDist]
	;fstp	d [Letter1_Obj + O_POS + 4]


	; init objects
	mov	edi, o LetTab1_Obj
	mov	ebx, o LetTab1_Strc
	mov	ecx, W1_TEXT3D_LEN
W1I_obj:
	mov	d [edi + O_PTR_STRUCT3D], ebx
	mov	eax, d [_ar_x]
	mov	d [edi + O_ADD_ANGLE_X], eax
	mov	eax, d [_ar_y]
	mov	d [edi + O_ADD_ANGLE_Y], eax
	mov	eax, d [_ar_z]
	mov	d [edi + O_ADD_ANGLE_Z], eax
	mov	d [edi + O_FLAGS], OF_MOVEABLE
	push	edi
	push	ebx
	push	ecx
	mov	esi, ebx
	call	GetStruct3dRadius
	pop	ecx
	pop	ebx
	pop	edi
	fld	st(0)
	fadd	d [MinObjDist]
	fstp	d [edi + O_POS + 4]
	fld	d [_lastrad]
	fst	d [edi + O_POS + 0]
	faddp	st(1), st(0)
	fadd	d [MinObjDist]
	fstp	d [_lastrad]
	add	edi, OBJECT3D_SIZE
	add	ebx, STRUCT3D_SIZE
	loop	W1I_obj


	; init camera
	mov	esi, o W1_TrackV1
	mov	edi, o W1_TrackV2
	mov	ecx, T_STD_NUM_OF_VERTS
	mov	ebx, World1_Track1
	mov	eax, d [World1_Camera + CAM_PTR_TRACK]
	push	eax
	call	MakeLinearTrack

	mov	esi, o W1_TrackV2
	mov	edi, o W1_TrackV3
	mov	ecx, T_STD_NUM_OF_VERTS
	mov	ebx, World1_Track2
	pop	eax
	add	eax, (T_STD_NUM_OF_VERTS * 12)
	push	eax
	call	MakeLinearTrack

	mov	esi, o W1_TrackV3
	mov	edi, o W1_TrackV4
	mov	ecx, T_STD_NUM_OF_VERTS
	mov	ebx, World1_Track3
	pop	eax
	add	eax, (T_STD_NUM_OF_VERTS * 12)
	push	eax
	call	MakeLinearTrack

	mov	esi, o W1_TrackV4
	mov	edi, o W1_TrackV1
	mov	ecx, T_STD_NUM_OF_VERTS
	mov	ebx, World1_Track4
	pop	eax
	add	eax, (T_STD_NUM_OF_VERTS * 12)
	call	MakeLinearTrack

	mov	esi, d [World1_Camera + CAM_PTR_TRACK]
	mov	ecx, T_STD_NUM_OF_VERTS * 4
	mov	edi, o World1_Camera
	mov	eax, CF_NO_TARGET
	call	SetTrack

	pop	eax
	ret

;************************************************************

; this is because World2 uses Floor1 as plasma
World1_FixFloor:
	mov	edi, d [Floor1_Strc + S3D_PTR_B_VERTS]
	mov	ecx, FLOOR1_N_VERTS
	xor	eax, eax
W1FF_do:
	mov	d [edi + 4], eax
	add	edi, 12
	loop	W1FF_do
	ret

;************************************************************

MinObjDist	dd 10.0
_mem111		dd 0
;_anglerot	dd 0.017
_lastrad	dd 0

_ar_x		dd 0.020
_ar_y		dd 0.015
_ar_z		dd 0.010
