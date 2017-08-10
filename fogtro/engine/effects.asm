
;************************************************************
;	PlasmaFloor() (unoptimized !)
;
;	in: esi = ptr to struct3d, eax = floor side
;************************************************************
PlasmaFloor:
	mov	ebp, eax

	sub	esp, 256 * 4
	mov	ebx, esp

	xor	eax, eax
	mov	ecx, 256
	fldz
PF_GenSin:
	fld	st(0)
	fsin
	fmul	d [PF_A]
	fstp	d [ebx + eax]
	add	eax, 4
	fadd	d [PF_angle]
	dec	ecx
	jnz	PF_GenSin
	ffree	st(0)

	mov	edi, d [esi + S3D_PTR_B_VERTS]
	xor	edx, edx
	mov	ax, w [PF_1]
	mov	w [PF_t1], ax
PF_y:
	xor	ecx, ecx
	mov	ax, w [PF_3]
	mov	w [PF_t3], ax
PF_x:
	movzx	eax, b [PF_t1]
	shl	eax, 2
	fld	d [ebx + eax]
	movzx	eax, b [PF_t2]
	shl	eax, 2
	fadd	d [ebx + eax]
	movzx	eax, b [PF_t3]
	shl	eax, 2
	fadd	d [ebx + eax]
	movzx	eax, b [PF_t4]
	shl	eax, 2
	fadd	d [ebx + eax]

	fstp	d [edi + 4]
	
	add	b [PF_t3], 6 * 6
	add	b [PF_t4], -7 * 6

	add	edi, 12
	inc	ecx
	cmp	ecx, ebp
	jne	PF_x

	add	b [PF_t1], 3 * 6
	add	b [PF_t2], 4 * 6

	inc	edx
	cmp	edx, ebp
	jne	PF_y

	add	b [PF_1], 1
	add	b [PF_2], 4
	add	b [PF_3], -2
	add	b [PF_4], 5

	add	esp, 256 * 4
	ret

;************************************************************

PF_A		dd 20.0
PF_angle	dd 0.0245 ; pi/128

PF_1		db 0
PF_2		db 0
PF_3		db 0
PF_4		db 0

PF_t1		db 0
PF_t2		db 0
PF_t3		db 0
PF_t4		db 0
