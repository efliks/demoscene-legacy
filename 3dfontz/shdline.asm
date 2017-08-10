
; in: eax = x1 << GT_SHIFT, ebx = x2 << GT_SHIFT
; ecx = y
ShadowFlatLine:
	push	eax
	push	ebx
	push	ecx

	sar	eax, GT_SHIFT
	sar	ebx, GT_SHIFT

	add	eax, SHADOW_X
	add	ebx, SHADOW_X
	add	ecx, SHADOW_Y

	or	ecx, ecx
	jl	SFL_quit
	cmp	ecx, 199
	jg	SFL_quit

	or	eax, eax
	jge	SFL_ok1
	xor	eax, eax
	jmp	SFL_ok2
SFL_ok1:
	cmp	eax, 319
	jle	SFL_ok2
	mov	eax, 319
SFL_ok2:
	or	ebx, ebx
	jge	SFL_ok3
	xor	ebx, ebx
	jmp	SFL_ok4
SFL_ok3:
	cmp	ebx, 319
	jle	SFL_ok4
	mov	ebx, 319
SFL_ok4:

	cmp	eax, ebx
	jl	SFL_ok
	je	SFL_quit

	xchg	eax, ebx
SFL_ok:
	mov	edi, ecx
	shl	edi, 6
	shl	ecx, 8
	add	edi, ecx
	add	edi, eax
	add	edi, d [FrameBuffer]

	mov	ecx, ebx
	sub	ecx, eax
SFL_draw:
	mov	al, b [edi]
	cmp	al, 128
	jb	SFL_next
	and	al, 31
	add	al, 128
	mov	b [edi], al
SFL_next:
	inc	edi
	dec	ecx
	jge	SFL_draw
SFL_quit:
	pop	ecx
	pop	ebx
	pop	eax
	ret
