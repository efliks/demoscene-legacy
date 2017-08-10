.386p
.model flat

include 3dfontz.inc
include letgen.inc
include engine.asm
include sysfont.inc

.code

lg_code:


_MakeLetter3d:
	push	ebp
	mov	ebp, esp

	push	eax
	mov	eax, d [ebp + 8]
	call	MakeLetter3d
	pop	eax

	mov	esp, ebp
	pop	ebp
	ret

;------------------------------------------------------------

; out: eax = Buffer3d address
_EnableBuffer3d:
	mov	eax, o Buffer3d_v
	mov	d [Buffer3d.ts3d_points], eax

	mov	eax, o Buffer3d_f
	mov	d [Buffer3d.ts3d_faces], eax

	mov	eax, o Buffer3d
	ret

;------------------------------------------------------------

; in: eax = ASCII number
MakeLetter3d:
	pushad

	and	eax, 255
	shl	eax, 3
	add	eax, o FontTable ;d [FontAddress]
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

	mov	edi, o Buffer3d
	call	CenterStruct3d

	mov	edi, o Buffer3d
	call	ScaleStruct3d

	popad
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


LG_CODE_SIZE	equ $ - lg_code

;------------------------------------------------------------

.data

lg_data:

CubeVertices	dd -CSIZE,-CSIZE,CSIZE, CSIZE,-CSIZE,CSIZE, CSIZE,CSIZE,CSIZE
		dd -CSIZE,CSIZE,CSIZE, -CSIZE,-CSIZE,-CSIZE, CSIZE,-CSIZE,-CSIZE
		dd CSIZE,CSIZE,-CSIZE, -CSIZE,CSIZE,-CSIZE

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

; LetMul = CSIZE * 2
LetMul		dd 64.0

Points3dSize	dd ?
Points2dSize	dd ?
FacesSize	dd ?
TablesSize	dd ?

Buffer3d_v	point3d (N_POINTS * (size point3d)) dup(?)
Buffer3d_f	face (N_FACES * (size face)) dup(?)


LG_DATA_SIZE	equ $ - lg_data

end
