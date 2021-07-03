
; in: edi = ptr to 320x200 buffer
GenerateSineTexture:
	push	ebp
	mov	ebp, esp
	sub	esp, 256 + 4

	mov	edx, esp			; edx points to temp32
	mov	ebx, edx
	add	ebx, 4				; ebx points to lookup table

	fldz
	xor	esi, esi
	mov	ecx, 256
GST_lookup:
	fld	st
	fcos
	fmul	d [GST_mul]
	fistp	d [edx]
	mov	eax, d [edx]
	mov	b [ebx + esi], al
	inc	esi
	fadd	d [pi_128]
	loop	GST_lookup
	ffree	st

	cld
	push	edi
	mov	edx, 200
GST_v:
	mov	ecx, 256
GST_h:
	movzx	esi, b [a1]
	mov	al, b [ebx + esi]
	movzx	esi, b [a2]
	add	al, b [ebx + esi]
	movzx	esi, b [a3]
	add	al, b [ebx + esi]
	movzx	esi, b [a4]
	add	al, b [ebx + esi]
	and	al, 63
	add	al, 128 + 64
	
	stosb
	add	b [a1], 3
	add	b [a2], 2
	loop	GST_h

	mov	esi, edi
	sub	esi, 256
	mov	ecx, (320 - 256) / 4
	rep	movsd

	add	b [a3], 1
	add	b [a4], 5
	dec	edx
	jnz	GST_v

	pop	edi
	;call	blurer

	mov	esp, ebp
	pop	ebp
	ret

;------------------------------------------------------------

pi_128		dd 0.025
GST_mul		dd 31.0

a1		db 33
a2		db 99
a3		db 27
a4		db 21
