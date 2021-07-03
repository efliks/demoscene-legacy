;------------------------------------------------------------
;	3D Letters Generator 1.6 by Majuma/NAAG
;	Mail: majuma@wp.pl
;	Web: www.ps.nq.pl / www.naag.prv.pl
;
;	Updated: 05/02/02, 16/02/02, 26/03/02, 08/08/02, 23/12/02,
;	18/02/03
;------------------------------------------------------------

lg_code:

; in: eax = ASCII number
; out: eax = number of vertices, ebx = number of faces
GetLetterParams:
	mov	b [GetParamsMode], 1

; //in: eax = ASCII number
; //(esi = ptr to memory handle, edi = ptr to struct3d) <- UNUSED !
; //out: eax = 0 if error occured
MakeLetter3d:

; in: eax = ASCII number, ebx = letter size, esi = ptr to initialized struct3d
LG_GenerateLetter:
	; allocate memory on stack !
	sub	esp, BUFFER3D_SIZE
	mov	d [Buffer3d.ts3d_points], esp
	mov	d [Buffer3d.ts3d_faces], esp
	add	d [Buffer3d.ts3d_faces], N_POINTS * (size point3d)

	mov	d [_is3d], esi
	mov	d [_size], ebx
	pushad

	and	eax, 255
	shl	eax, 3
	add	eax, d [FontAddress]
	mov	esi, eax

	mov	d [Buffer3d.ts3d_n_points], 0
	mov	d [Buffer3d.ts3d_n_faces], 0

	mov	edi, d [Buffer3d.ts3d_points]
	mov	ebp, d [Buffer3d.ts3d_faces]

	cld
	xor	edx, edx
ML3d_v:
	xor	ecx, ecx
	lodsb
ML3d_h:
	rcl	al, 1
	jnc	ML3d_next

	push	eax
	push	ecx
	push	edx
	push	esi

	mov	d [CurrCubePos.x3d], ecx
	fild	d [CurrCubePos.x3d]
	fmul	d [LetMul]
	fstp	d [CurrCubePos.x3d]

	mov	d [CurrCubePos.y3d], edx
	fild	d [CurrCubePos.y3d]
	fmul	d [LetMul]
	fstp	d [CurrCubePos.y3d]

	mov	d [CurrCubePos.z3d], 0

;------------------------------------------------------------
;	write vertices

	mov	ebx, o CubeVertices
	xor	ecx, ecx
	mov	d [VerticesToAdd], 0
ML3d_vert:
	fld	d [ebx.x3d]
	fadd	d [CurrCubePos.x3d]
	fstp	d [CurrPoint.x3d]
	fld	d [ebx.y3d]
	fadd	d [CurrCubePos.y3d]
	fstp	d [CurrPoint.y3d]
	fld	d [ebx.z3d]
	fadd	d [CurrCubePos.z3d]
	fstp	d [CurrPoint.z3d]


	mov	edx, d [Buffer3d.ts3d_n_points]
	or	edx, edx
	jz	ML3d_add_vert

	mov	esi, d [Buffer3d.ts3d_points]
	xor	edx, edx
ML3d_check_vert:
	fld	d [esi.x3d]
	fcomp	d [CurrPoint.x3d]
	fstsw	ax
	sahf
	jne	ML3d_check_next
	fld	d [esi.y3d]
	fcomp	d [CurrPoint.y3d]
	fstsw	ax
	sahf
	jne	ML3d_check_next
	fld	d [esi.z3d]
	fcomp	d [CurrPoint.z3d]
	fstsw	ax
	sahf
	jne	ML3d_check_next

	mov	w [FaceVertices + ecx*2], dx
	jmp	ML3d_next_vert
ML3d_check_next:
	add	esi, size point3d
	inc	edx
	cmp	edx, d [Buffer3d.ts3d_n_points]
	jne	ML3d_check_vert

ML3d_add_vert:
	mov	eax, d [CurrPoint.x3d]
	mov	d [edi.x3d], eax
	mov	eax, d [CurrPoint.y3d]
	mov	d [edi.y3d], eax
	mov	eax, d [CurrPoint.z3d]
	mov	d [edi.z3d], eax

	add	edi, size point3d

	mov	eax, d [Buffer3d.ts3d_n_points]
	add	eax, d [VerticesToAdd]
	mov	w [FaceVertices + ecx*2], ax
	
	inc	d [VerticesToAdd]
ML3d_next_vert:
	add	ebx, size point3d
	inc	ecx
	cmp	ecx, 8
	jne	ML3d_vert

;------------------------------------------------------------
;	write faces

	mov	ebx, o CubeFaces
	mov	ecx, 12 / 2
	mov	d [FacesToAdd], 0
ML3d_poly:
	movzx	eax, w [ebx.face_v1]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace1.face_v1], ax

	movzx	eax, w [ebx.face_v2]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace1.face_v2], ax

	movzx	eax, w [ebx.face_v3]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace1.face_v3], ax

	movzx	eax, w [ebx.face_v1 + (size face)]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace2.face_v1], ax

	movzx	eax, w [ebx.face_v2 + (size face)]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace2.face_v2], ax

	movzx	eax, w [ebx.face_v3 + (size face)]
	mov	ax, w [FaceVertices + eax*2]
	mov	w [CurrFace2.face_v3], ax


	push	ebx
	push	edi
	mov	esi, o CurrFace1
	mov	edi, o CurrFace2
	mov	ebx, o CurrPolygon
	call	Faces2Poly
	pop	edi
	pop	ebx

	mov	edx, d [Buffer3d.ts3d_n_faces]
	or	edx, edx
	jz	ML3d_add_poly
	xor	edx, edx
	mov	esi, d [Buffer3d.ts3d_faces]
ML3d_check_poly:
	push	esi
	push	edi
	push	ebx

	mov	edi, esi
	add	edi, size face
	mov	ebx, o NextPolygon
	call	Faces2Poly

	mov	esi, o CurrPolygon
	mov	edi, o NextPolygon
	call	CmpPolys

	pop	ebx
	pop	edi
	pop	esi

	cmp	eax, POLYS_DIFF
	je	ML3d_check_next_poly

	sub	d [Buffer3d.ts3d_n_faces], 2
	sub	ebp, (size face) * 2

	push	edx
	push	edi

	mov	eax, edx
	shl	eax, 1
	shl	edx, 2
	add	eax, edx
	mov	edi, d [Buffer3d.ts3d_faces]
	add	edi, eax

	mov	ax, w [ebp.face_v1]
	mov	w [edi.face_v1], ax
	mov	ax, w [ebp.face_v2]
	mov	w [edi.face_v2], ax
	mov	ax, w [ebp.face_v3]
	mov	w [edi.face_v3], ax

	mov	ax, w [ebp.face_v1 + (size face)]
	mov	w [edi.face_v1 + (size face)], ax
	mov	ax, w [ebp.face_v2 + (size face)]
	mov	w [edi.face_v2 + (size face)], ax
	mov	ax, w [ebp.face_v3 + (size face)]
	mov	w [edi.face_v3 + (size face)], ax
	
	pop	edi
	pop	edx

	jmp	ML3d_next_poly
ML3d_check_next_poly:
	add	esi, (size face) * 2
	add	edx, 2
	cmp	edx, d [Buffer3d.ts3d_n_faces]
	jne	ML3d_check_poly


ML3d_add_poly:
	mov	ax, w [CurrFace1.face_v1]
	mov	w [ebp.face_v1], ax
	mov	ax, w [CurrFace1.face_v2]
	mov	w [ebp.face_v2], ax
	mov	ax, w [CurrFace1.face_v3]
	mov	w [ebp.face_v3], ax

	mov	ax, w [CurrFace2.face_v1]
	mov	w [ebp.face_v1 + (size face)], ax
	mov	ax, w [CurrFace2.face_v2]
	mov	w [ebp.face_v2 + (size face)], ax
	mov	ax, w [CurrFace2.face_v3]
	mov	w [ebp.face_v3 + (size face)], ax
	
	add	d [FacesToAdd], 2
	add	ebp, (size face) * 2
ML3d_next_poly:
	add	ebx, (size face) * 2
	dec	ecx
	jnz	ML3d_poly

;------------------------------------------------------------

	mov	eax, d [VerticesToAdd]
	add	d [Buffer3d.ts3d_n_points], eax
	
	mov	eax, d [FacesToAdd]
	add	d [Buffer3d.ts3d_n_faces], eax

	pop	esi
	pop	edx
	pop	ecx
	pop	eax
ML3d_next:
	inc	ecx
	cmp	ecx, 8
	jne	ML3d_h

	inc	edx
	cmp	edx, 8
	jne	ML3d_v

	; check if letter is empty
	cmp	d [Buffer3d.ts3d_n_points], 0
	jne	ML3d_not_empty

	mov	d [Buffer3d.ts3d_n_points], 8
	mov	d [Buffer3d.ts3d_n_faces], 12

	mov	esi, o CubeVertices
	mov	edi, d [Buffer3d.ts3d_points]
	mov	ecx, 8 * 3
	rep	movsd

	mov	esi, o CubeFaces
	mov	edi, d [Buffer3d.ts3d_faces]
	mov	ecx, 12 * 3
	rep	movsw
ML3d_not_empty:

	;mov	edi, o Buffer3d
	;fld	d [LG_ScaleX]
	;fld	d [LG_ScaleY]
	;fld	d [LG_ScaleZ]
	;call	ScaleStruct3d

	;popad
	;call	SaveLetter3d

	;call	Add2Struct3d

	
	mov	al, b [GetParamsMode]
	cmp	al, 1
	jne	ML3d_normal
	xor	al, al
	mov	b [GetParamsMode], al
	popad
	mov	eax, d [Buffer3d.ts3d_n_points]
	mov	ebx, d [Buffer3d.ts3d_n_faces]
	jmp	ML3d_leave
ML3d_normal:
	; copy letter from buffer to struct3d
	mov	ebx, d [_is3d]

	mov	ecx, d [Buffer3d.ts3d_n_points]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	mov	esi, d [Buffer3d.ts3d_points]
	mov	edi, d [ebx + S3D_PTR_B_VERTS]
	cld
	rep	movsd

	mov	ecx, d [Buffer3d.ts3d_n_faces]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	mov	esi, d [Buffer3d.ts3d_faces]
	mov	edi, d [ebx + S3D_PTR_FACES]
	rep	movsw

	; scale letter
	mov	esi, ebx
	push	ebx
	push	ebx
	call	NormalizeStruct3d

	pop	edi
	call	CenterStruct3dA

	pop	edi
	fild	d [_size]
	fld	st(0)
	fld	st(0)
	fadd	st(0), st(0) ; scale_z *= 2
	call	ScaleStruct3d

	popad
ML3d_leave:
	; free allocated memory !
	add	esp, BUFFER3D_SIZE
	ret

;------------------------------------------------------------

; in: esi = ptr to face1, edi = ptr to face2, ebx = ptr to polygon
Faces2Poly:
	push	eax
	push	ecx

	mov	ax, w [esi.face_v1]
	mov	w [ebx.poly_v1], ax
	mov	ax, w [esi.face_v2]
	mov	w [ebx.poly_v2], ax
	mov	ax, w [esi.face_v3]
	mov	w [ebx.poly_v3], ax

	mov	ecx, 3
F2P_check:
	mov	ax, w [edi]
	cmp	ax, w [esi.face_v1]
	je	F2P_next
	cmp	ax, w [esi.face_v2]
	je	F2P_next
	cmp	ax, w [esi.face_v3]
	je	F2P_next
	jmp	F2P_done
F2P_next:
	add	edi, 2
	loop	F2P_check
F2P_done:
	mov	w [ebx.poly_v4], ax

	pop	ecx
	pop	eax
	ret

;------------------------------------------------------------

; in: esi = ptr to 1st polygon, edi = ptr to 2nd polygon
; out: eax = POLYS_DIFF or POLYS_EQUAL
CmpPolys:
	pushad
	
	xor	edx, edx
CP_1:
	xor	ecx, ecx
	xor	ebx, ebx
CP_2:
	mov	ax, w [esi + edx*2]
	cmp	ax, w [edi + ecx*2]
	jne	CP_cont
	inc	ebx
	jmp	CP_2_done
CP_cont:
	inc	ecx
	cmp	ecx, 4
	jne	CP_2
CP_2_done:
	or	ebx, ebx
	jz	CP_not_found
	
	inc	edx
	cmp	edx, 4
	jne	CP_1

	popad
	mov	eax, POLYS_EQUAL
	ret
CP_not_found:
	popad
	mov	eax, POLYS_DIFF
	ret

;------------------------------------------------------------
comment #

; in: esi = ptr to memory handle, edi = ptr to struct3d
; out: eax = 0 if error occured
SaveLetter3d:
	pushad

	mov	eax, d [Buffer3d.ts3d_n_points]
	mov	d [edi.s3d_n_points], eax

	push	eax
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	mov	d [Points3dSize], eax
	shl	eax, 2				; (n_points * (size point3d) * 4)

	pop	ebx
	shl	ebx, 2				; (n_points * (size point2d))
	mov	d [Points2dSize], ebx

	add	eax, ebx
	push	eax

	mov	eax, d [Buffer3d.ts3d_n_faces]
	mov	d [edi.s3d_n_faces], eax

	push	eax
	mov	ebx, eax
	shl	eax, 1
	shl	ebx, 2
	add	eax, ebx			; (n_faces * (size face))
	mov	d [FacesSize], eax

	pop	ebx
	shl	ebx, 1
	mov	d [TablesSize], ebx
	shl	ebx, 1				; (n_faces * 2 * 2)
	add	eax, ebx

	pop	ebx
	add	eax, ebx

	push	esi
	push	edi
	call	alloc_mem
	or	eax, eax
	jnz	SL3d_mem_allocated
	pop	edi
	pop	esi
	popad
	xor	eax, eax
	ret
SL3d_mem_allocated:
	pop	edi
	pop	esi

	mov	d [esi], eax
	mov	eax, ebx

	mov	d [edi.s3d_points], eax
	add	eax, d [Points3dSize]
	mov	d [edi.s3d_r_points], eax
	add	eax, d [Points3dSize]
	mov	d [edi.s3d_t_points], eax
	add	eax, d [Points2dSize]
	mov	d [edi.s3d_faces], eax
	add	eax, d [FacesSize]
	mov	d [edi.s3d_depths], eax
	add	eax, d [TablesSize]
	mov	d [edi.s3d_order], eax
	add	eax, d [TablesSize]
	mov	d [edi.s3d_point_nrm], eax
	add	eax, d [Points3dSize]
	mov	d [edi.s3d_r_point_nrm], eax

	mov	ebp, edi

	mov	esi, d [Buffer3d.ts3d_points]
	mov	edi, d [ebp.s3d_points]
	mov	ecx, d [Buffer3d.ts3d_n_points]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	cld
	rep	movsd

	mov	esi, d [Buffer3d.ts3d_faces]
	mov	edi, d [ebp.s3d_faces]
	mov	ecx, d [Buffer3d.ts3d_n_faces]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	rep	movsw


	mov	esi, ebp
	call	init_point_normals

	popad
	mov	eax, 1
	ret #

;------------------------------------------------------------

comment #
; in: esi = ptr to memory handle, edi = ptr to struct3d
; ebx = ptr to struct3d to be cloned
; out: eax = 0 if error
CloneLetter3d:
	mov	ecx, d [ebx.s3d_n_points]
	mov	d [Buffer3d.ts3d_n_points], ecx
	mov	edx, d [ebx.s3d_n_faces]
	mov	d [Buffer3d.ts3d_n_faces], edx

	push	esi
	push	edi

	mov	esi, d [ebx.s3d_points]
	mov	edi, d [Buffer3d.ts3d_points]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	cld
	rep	movsd

	mov	esi, d [ebx.s3d_faces]
	mov	edi, d [Buffer3d.ts3d_faces]
	mov	ecx, edx
	shl	edx, 1
	add	ecx, edx
	rep	movsw

	pop	edi
	pop	esi

	call	SaveLetter3d
	ret

;------------------------------------------------------------

; out: eax = 0 if error
EnableBuffer3d:
	mov	eax, BUFFER3D_SIZE
	call	alloc_mem
	or	eax, eax
	jnz	SB3d_mem_ok
	ret
SB3d_mem_ok:
	mov	d [Buffer3d_handle], eax

	mov	d [Buffer3d.ts3d_points], ebx
	add	ebx, N_POINTS * (size point3d)
	mov	d [Buffer3d.ts3d_faces], ebx
	ret

;------------------------------------------------------------

DisableBuffer3d:
	mov	eax, d [Buffer3d_handle]
	call	free_mem
	ret #

;------------------------------------------------------------

LG_CODE_SIZE	equ $ - lg_code

; 3d letters generator constants
N_POINTS	equ 8 * 64
N_FACES		equ 12 * 64
BUFFER3D_SIZE	equ N_POINTS * (size point3d) + N_FACES * (size face)

CSIZE		equ 64.0
LETGEN_SCALEX	equ 1.0 ;0.1
LETGEN_SCALEY	equ 1.0 ;0.1
LETGEN_SCALEZ	equ 1.0 ;0.25

POLYS_EQUAL	equ 1
POLYS_DIFF	equ -1


lg_data:

GetParamsMode	db 0
_is3d		dd 0
_size		dd 0


CubeVertices	dd 0,0,CSIZE, CSIZE,0,CSIZE, CSIZE,CSIZE,CSIZE
		dd 0,CSIZE,CSIZE, 0,0,0, CSIZE,0,0
		dd CSIZE,CSIZE,0, 0,CSIZE,0

CubeFaces	dw 0,1,2, 0,2,3, 1,5,6, 1,6,2, 4,0,3, 4,3,7, 4,5,1, 4,1,0
		dw 3,2,6, 3,6,7, 5,4,7, 5,7,6

Buffer3d_handle	dd ?
Buffer3d	tiny_struct3d ?

CurrCubePos	point3d ?
CurrPoint	point3d ?

VerticesToAdd	dd ?
FaceVertices	dw 8 dup(?)

FacesToAdd	dd ?
CurrFace1	face ?
CurrFace2	face ?
CurrPolygon	polygon ?
NextPolygon	polygon ?

; LetMul = CSIZE
LetMul		dd 64.0

Points3dSize	dd ?
Points2dSize	dd ?
FacesSize	dd ?
TablesSize	dd ?

LG_ScaleX	dd LETGEN_SCALEX
LG_ScaleY	dd LETGEN_SCALEY
LG_ScaleZ	dd LETGEN_SCALEZ

LG_DATA_SIZE	equ $ - lg_data
