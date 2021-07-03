
; in: eax = ASCII code
; out: eax = requested bytes
; esi = number of vertices, edi = number of faces
GetLetterMem:
	mov	ebp, eax

	movzx	eax, w [FontTable + ebp*4 + 0]
	mov	esi, eax
	push	eax

	shl	eax, 2
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx

	pop	ebx
	shl	ebx, 2
	add	eax, ebx
	push	eax

	movzx	eax, w [FontTable + ebp*4 + 2]
	mov	edi, eax
	push	eax

	mov	ebx, eax
	shl	eax, 1
	shl	ebx, 2
	add	eax, ebx

	pop	ebx
	shl	ebx, 2
	add	eax, ebx

	pop	ebx
	add	eax, ebx
	ret

;------------------------------------------------------------

; in: esi = ptr to ASCIIZ string, ecx = string's lenght
; out: eax = requested bytes
; esi = number of vertices, edi = number of faces
GetStringMem:
	cld
	xor	edx, edx	; bytes
	xor	ebx, ebx	; vertices
	xor	ebp, ebp	; faces
BS_loop:
	lodsb
	movzx	eax, al
	push	esi

	push	ebx
	push	edx
	push	ebp

	call	GetLetterMem
	
	pop	ebp
	pop	edx
	pop	ebx

	add	edx, eax
	add	ebx, esi
	add	ebp, edi

	pop	esi
	loop	BS_loop

	mov	eax, edx
	mov	d [MemSize], edx
	mov	esi, ebx
	mov	d [max_verts], ebx
	mov	edi, ebp
	mov	d [max_faces], ebp
	ret

;------------------------------------------------------------

; in: eax = ptr to allocated memory block, edi = ptr to struct3d
SetAddress2Struct3d:
	mov	ecx, d [max_verts]
	mov	ebx, ecx
	shl	ecx, 2
	shl	ebx, 3
	add	ecx, ebx
	push	ecx

	mov	d [edi.s3d_points], eax
	add	eax, ecx
	mov	d [edi.s3d_r_points], eax
	add	eax, ecx

	mov	ecx, d [max_verts]
	shl	ecx, 2

	mov	d [edi.s3d_t_points], eax
	add	eax, ecx

	mov	ecx, d [max_faces]
	mov	ebx, ecx
	shl	ecx, 1
	shl	ebx, 2
	add	ecx, ebx

	mov	d [edi.s3d_faces], eax
	add	eax, ecx

	mov	ecx, d [max_faces]
	shl	ecx, 1

	mov	d [edi.s3d_depths], eax
	add	eax, ecx
	mov	d [edi.s3d_order], eax
	add	eax, ecx

	pop	ecx
	mov	d [edi.s3d_point_nrm], eax
	add	eax, ecx
	mov	d [edi.s3d_r_point_nrm], eax
	ret

;------------------------------------------------------------

Add2Struct3d:
	mov	ebp, d [BigStructPtr]

	mov	eax, d [Buffer3d.ts3d_n_points]
	add	d [ebp.s3d_n_points], eax
	mov	eax, d [Buffer3d.ts3d_n_faces]
	add	d [ebp.s3d_n_faces], eax

	; copy vertices
	mov	ecx, d [Buffer3d.ts3d_n_points]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	mov	esi, d [Buffer3d.ts3d_points]
	mov	edi, d [ebp.s3d_points]
	add	edi, d [VertsOfs]
	cld
	rep	movsd

	; copy faces
	mov	ecx, d [Buffer3d.ts3d_n_faces]
	mov	eax, ecx
	shl	ecx, 1
	add	ecx, eax
	mov	esi, d [Buffer3d.ts3d_faces]
	mov	edi, d [ebp.s3d_faces]
	add	edi, d [FacesOfs]
A2S3d_copy_face:
	lodsw
	add	ax, w [Verts2Add]
	stosw
	dec	ecx
	jnz	A2S3d_copy_face

	; copy to small struct
	mov	edx, d [SmallStructPtr]

	mov	eax, d [Buffer3d.s3d_n_points]
	mov	d [edx.s3d_n_points], eax

	mov	eax, d [ebp.s3d_points]
	add	eax, d [VertsOfs]
	mov	d [edx.s3d_points], eax

	mov	eax, d [ebp.s3d_r_points]
	add	eax, d [VertsOfs]
	mov	d [edx.s3d_r_points], eax

	mov	eax, d [ebp.s3d_t_points]
	add	eax, d [Verts2dOfs]
	mov	d [edx.s3d_t_points], eax

	mov	eax, d [ebp.s3d_point_nrm]
	add	eax, d [VertsOfs]
	mov	d [edx.s3d_point_nrm], eax

	mov	eax, d [ebp.s3d_r_point_nrm]
	add	eax, d [VertsOfs]
	mov	d [edx.s3d_r_point_nrm], eax


	mov	eax, d [Buffer3d.ts3d_n_points]
	push	eax
	add	w [Verts2Add], ax

	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	d [VertsOfs], eax

	pop	eax
	shl	eax, 2
	add	d [Verts2dOfs], eax

	mov	eax, d [Buffer3d.ts3d_n_faces]
	mov	ebx, eax
	shl	eax, 1
	shl	ebx, 2
	add	eax, ebx
	add	d [FacesOfs], eax

	inc	d [CurrLetter]
	add	d [SmallStructPtr], size struct3d
	ret

;------------------------------------------------------------

; in: esi = ptr to string, edi = ptr to structs, ebx = ptr to big struct
; eax = ptr to allocated memory, ecx = string's lenght
MassiveMakeString3d:
	push	ebx
	push	ecx
	push	edx
	push	edi
	mov	edi, ebx
	call	SetAddress2Struct3d
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx

	mov	d [BigStructPtr], ebx
	mov	d [SmallStructPtr], edi

	mov	d [ebx.s3d_n_points], 0
	mov	d [ebx.s3d_n_faces], 0
	
	xor	eax, eax
	mov	d [CurrLetter], eax
	mov	d [VertsOfs], eax
	mov	d [Verts2dOfs], eax
	mov	d [FacesOfs], eax
	mov	d [Verts2Add], eax

	cld
MMS3d_letter:
	lodsb
	movzx	eax, al
	push	esi
	push	ecx
	call	MakeLetter3d
	pop	ecx
	pop	esi
	loop	MMS3d_letter

	mov	esi, d [BigStructPtr]
	call	init_point_normals
	ret

;------------------------------------------------------------

BigStructPtr	dd ?
SmallStructPtr	dd ?
CurrLetter	dd ?

MemSize		dd ?
max_verts	dd ?
max_faces	dd ?

VertsOfs	dd ?
Verts2dOfs	dd ?
FacesOfs	dd ?
Verts2Add	dd ?
