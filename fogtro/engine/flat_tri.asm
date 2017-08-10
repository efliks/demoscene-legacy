
;------------------------------------------------------------
;	in:	filled t_strc
;	out:	none
;------------------------------------------------------------

flat_triangle proc

	mov	eax, t_y1
	cmp	eax, t_y3
	jle	@@ok13

	xchg	eax, t_y3
	mov	t_y1, eax

	mov	eax, t_x1
	xchg	eax, t_x3
	mov	t_x1, eax
@@ok13:

	mov	eax, t_y2
	cmp	eax, t_y3
	jle	@@ok23

	xchg	eax, t_y3
	mov	t_y2, eax

	mov	eax, t_x2
	xchg	eax, t_x3
	mov	t_x2, eax
@@ok23:

	mov	eax, t_y1
	cmp	eax, t_y2
	jle	@@ok12

	xchg	eax, t_y2
	mov	t_y1, eax

	mov	eax, t_x1
	xchg	eax, t_x2
	mov	t_x1, eax
@@ok12:
	
	mov	ebx, t_y2
	sub	ebx, t_y1
	jnz	@@make_d12

	mov	t_dx12, 0
	jmp	@@done_d12
@@make_d12:
	mov	eax, t_x2
	sub	eax, t_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	t_dx12, eax
@@done_d12:

	mov	ebx, t_y3
	sub	ebx, t_y1
	jnz	@@make_d13

	mov	t_dx13, 0
	jmp	@@done_d13
@@make_d13:
	mov	eax, t_x3
	sub	eax, t_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	t_dx13, eax
@@done_d13:

	mov	ebx, t_y3
	sub	ebx, t_y2
	jnz	@@make_d23

	mov	t_dx23, 0
	jmp	@@done_d23
@@make_d23:
	mov	eax, t_x3
	sub	eax, t_x2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	t_dx23, eax
@@done_d23:
	
	mov	eax, t_x1
	shl	eax, SHIFT_CONST
	mov	ebx, eax

	mov	ecx, t_y1
	cmp	ecx, t_y2
	jge	@@end_loop12
@@loop12:
	call	flat_line

	add	eax, t_dx13
	add	ebx, t_dx12
	inc	ecx
	cmp	ecx, t_y2
	jl	@@loop12
@@end_loop12:

	mov	ecx, t_y2
	cmp	ecx, t_y3
	jge	@@end_loop23

	mov	ebx, t_x2
	shl	ebx, SHIFT_CONST
@@loop23:
	call	flat_line

	add	eax, t_dx13
	add	ebx, t_dx23
	inc	ecx
	cmp	ecx, t_y3
	jl	@@loop23
@@end_loop23:

	ret
endp

;------------------------------------------------------------
;	in:	eax - x1 << SHIFT_CONST
;		ebx - x2 << SHIFT_CONST
;		ecx - y
;		t_col
;	out:	none
;------------------------------------------------------------

flat_line proc
	push	eax
	push	ebx
	push	ecx

	; vertical clipping
	or	ecx, ecx
	jl	@@quit
	cmp	ecx, 199
	jg	@@quit

	sar	eax, SHIFT_CONST
	sar	ebx, SHIFT_CONST

	; horizontal clipping
	or	eax, eax
	jge	@@ok1
	xor	eax, eax
	jmp	@@ok2
@@ok1:
	cmp	eax, 319 + 1
	jle	@@ok2
	mov	eax, 319 + 1
@@ok2:

	or	ebx, ebx
	jge	@@ok3
	xor	ebx, ebx
	jmp	@@ok4
@@ok3:
	cmp	ebx, 319 + 1
	jle	@@ok4
	mov	ebx, 319 + 1
@@ok4:

	cmp	eax, ebx
	jl	@@ok
	je	@@quit

	xchg	eax, ebx
@@ok:
	mov	edi, ecx
	shl	edi, 6
	shl	ecx, 8
	add	edi, ecx
	add	edi, eax
	add	edi, FrameBuffer

	mov	ecx, ebx
	sub	ecx, eax

	mov	eax, t_col
	mov	ah, al

	shr	ecx, 1
	jnc	@@draw
	stosb
@@draw:
	rep	stosw
@@quit:

	pop	ecx
	pop	ebx
	pop	eax
	ret
endp

t_dx12	dd ?
t_dx13	dd ?
t_dx23	dd ?

t_strc:
t_x1	dd ?
t_y1	dd ?
t_x2	dd ?
t_y2	dd ?
t_x3	dd ?
t_y3	dd ?
t_col	dd ?
