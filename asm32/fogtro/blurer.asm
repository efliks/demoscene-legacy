
; in: edi = ptr to 320x200 bitmap
blurer:
	xor	eax, eax
	mov	ecx, 320 / 4
	cld
	rep	stosd

	xor	ebx, ebx
	mov	edx, 198
b_v:
	mov	ecx, 318
	stosb
b_h:
	mov	al, b [edi - 1]
	mov	bl, b [edi + 1]
	add	eax, ebx
	mov	bl, b [edi - 320]
	add	eax, ebx
	mov	bl, b [edi + 320]
	add	eax, ebx
	shr	eax, 2
	stosb
	dec	ecx
	jnz	b_h

	xor	eax, eax
	stosb
	dec	edx
	jnz	b_v

	xor	eax, eax
	mov	ecx, 320 / 4
	rep	stosd
	ret

comment #
blurer:
	xor	eax, eax
	mov	ecx, 320 / 4
	cld
	rep	stosd

	mov	ecx, 64000 - 320*2
	xor	ebx, ebx
b_loop:
	mov	al, b [edi - 1]
	mov	bl, b [edi + 1]
	add	eax, ebx
	mov	bl, b [edi - 320]
	add	eax, ebx
	mov	bl, b [edi + 320]
	add	eax, ebx
	shr	eax, 2
	stosb
	dec	ecx
	jnz	b_loop

	xor	eax, eax
	mov	ecx, 320 / 4
	rep	stosd
	ret #
