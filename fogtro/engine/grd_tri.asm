
GouraudTriangle:
	mov	eax, d [GrdY1]
	cmp	eax, d [GrdY3]
	jle	GT_ok13

	xchg	eax, d [GrdY3]
	mov	d [GrdY1], eax

	mov	eax, d [GrdX1]
	xchg	eax, d [GrdX3]
	mov	d [GrdX1], eax

	mov	eax, d [GrdCol1]
	xchg	eax, d [GrdCol3]
	mov	d [GrdCol1], eax
GT_ok13:

	mov	eax, d [GrdY2]
	cmp	eax, d [GrdY3]
	jle	GT_ok23

	xchg	eax, d [GrdY3]
	mov	d [GrdY2], eax

	mov	eax, d [GrdX2]
	xchg	eax, d [GrdX3]
	mov	d [GrdX2], eax

	mov	eax, d [GrdCol2]
	xchg	eax, d [GrdCol3]
	mov	d [GrdCol2], eax
GT_ok23:

	mov	eax, d [GrdY1]
	cmp	eax, d [GrdY2]
	jle	GT_ok12

	xchg	eax, d [GrdY2]
	mov	d [GrdY1], eax

	mov	eax, d [GrdX1]
	xchg	eax, d [GrdX2]
	mov	d [GrdX1], eax

	mov	eax, d [GrdCol1]
	xchg	eax, d [GrdCol2]
	mov	d [GrdCol1], eax
GT_ok12:


	mov	ebx, d [GrdY2]
	sub	ebx, d [GrdY1]
	jnz	GT_d12_make

	mov	d [GrdDeltaX12], 0
	mov	d [GrdDeltaC12], 0
	jmp	GT_d12_done
GT_d12_make:
	mov	eax, d [GrdX2]
	sub	eax, d [GrdX1]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaX12], eax

	mov	eax, d [GrdCol2]
	sub	eax, d [GrdCol1]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaC12], eax
GT_d12_done:

	mov	ebx, d [GrdY3]
	sub	ebx, d [GrdY1]
	jnz	GT_d13_make

	mov	d [GrdDeltaX13], 0
	mov	d [GrdDeltaC13], 0
	jmp	GT_d13_done
GT_d13_make:
	mov	eax, d [GrdX3]
	sub	eax, d [GrdX1]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaX13], eax

	mov	eax, d [GrdCol3]
	sub	eax, d [GrdCol1]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaC13], eax
GT_d13_done:

	mov	ebx, d [GrdY3]
	sub	ebx, d [GrdY2]
	jnz	GT_d23_make

	mov	d [GrdDeltaX23], 0
	mov	d [GrdDeltaC23], 0
	jmp	GT_d23_done
GT_d23_make:
	mov	eax, d [GrdX3]
	sub	eax, d [GrdX2]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaX23], eax

	mov	eax, d [GrdCol3]
	sub	eax, d [GrdCol2]
	shl	eax, GT_SHIFT
	cdq
	idiv	ebx
	mov	d [GrdDeltaC23], eax
GT_d23_done:


	mov	eax, d [GrdX1]
	shl	eax, GT_SHIFT
	mov	ebx, eax

	mov	edx, d [GrdCol1]
	shl	edx, GT_SHIFT
	mov	ebp, edx

	mov	ecx, d [GrdY1]
	cmp	ecx, d [GrdY2]
	jge	GT_end_loop12
GT_loop12:
	call	GouraudClippedLine

	add	eax, d [GrdDeltaX13]
	add	ebx, d [GrdDeltaX12]
	add	edx, d [GrdDeltaC13]
	add	ebp, d [GrdDeltaC12]
	inc	ecx
	cmp	ecx, d [GrdY2]
	jl	GT_loop12
GT_end_loop12:

	mov	ecx, d [GrdY2]
	cmp	ecx, d [GrdY3]
	jge	GT_end_loop23

	mov	ebx, d [GrdX2]
	shl	ebx, GT_SHIFT

	mov	ebp, d [GrdCol2]
	shl	ebp, GT_SHIFT
GT_loop23:
	call	GouraudClippedLine

	add	eax, d [GrdDeltaX13]
	add	ebx, d [GrdDeltaX23]
	add	edx, d [GrdDeltaC13]
	add	ebp, d [GrdDeltaC23]
	inc	ecx
	cmp	ecx, d [GrdY3]
	jl	GT_loop23
GT_end_loop23:
	ret

;------------------------------------------------------------

; in: eax = x1 << GT_SHIFT, ebx = x2 << GT_SHIFT, ecx = y
; edx = c1 << GT_SHIFT, ebp = c2 << GT_SHIFT
GouraudClippedLine:
	or	ecx, ecx
	jl	GL_ret
	cmp	ecx, 199
	jg	GL_ret

	pushad

	sar	eax, GT_SHIFT
	sar	ebx, GT_SHIFT

	cmp	eax, ebx
	jl	GL_ok
	je	GL_quit

	xchg	eax, ebx
	xchg	edx, ebp
GL_ok:
	mov	edi, ecx
	shl	edi, 6
	shl	ecx, 8
	add	edi, ecx
	add	edi, d [FrameBuffer]	; edi = FrameBuffer + y * 320

	mov	ecx, ebx
	sub	ecx, eax		; ecx = x2 - x1 = lenght

	xor	esi, esi

	or	eax, eax
	jge	GL_ok1
	mov	esi, eax
	neg	esi
	xor	eax, eax
	jmp	GL_ok2
GL_ok1:
	cmp	eax, 319
	jle	GL_ok2
	mov	eax, 319
GL_ok2:

	or	ebx, ebx
	jge	GL_ok3
	xor	ebx, ebx
	jmp	GL_ok4
GL_ok3:
	cmp	ebx, 319
	jle	GL_ok4
	mov	ebx, 319
GL_ok4:

	cmp	eax, ebx
	je	GL_quit

	add	edi, eax		; edi += x1
	push	eax
	push	ebx

	mov	ebx, edx		; ebx = c1
	push	ebx

	mov	eax, ebp
	sub	eax, ebx		; eax = c2 - c1
	cdq
	idiv	ecx
	mov	ebp, eax		; ebp = delta_c = (c2 - c1) / lenght

	mov	eax, esi
	imul	ebp

	pop	esi			; esi = c1
	add	esi, eax		; c1 += (delta_c * pixels needed to
					; reach 0 if x1 < 0)

	pop	ebx
	mov	ecx, ebx
	pop	eax
	sub	ecx, eax		; ecx = x2 - x1
GL_draw:
	mov	eax, esi
	sar	eax, GT_SHIFT
	stosb
	add	esi, ebp
	dec	ecx
	jge	GL_draw
GL_quit:
	popad
GL_ret:
	ret

;------------------------------------------------------------

GrdX1		dd ?
GrdY1		dd ?
GrdX2		dd ?
GrdY2		dd ?
GrdX3		dd ?
GrdY3		dd ?

GrdCol1		dd ?
GrdCol2		dd ?
GrdCol3		dd ?


GrdDeltaX12	dd ?
GrdDeltaX13	dd ?
GrdDeltaX23	dd ?

GrdDeltaC12	dd ?
GrdDeltaC13	dd ?
GrdDeltaC23	dd ?
