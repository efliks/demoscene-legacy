.code

;------------------------------------------------------------
;	in:	x1, y1, x2, y2, x3, y3, c1, c2, c3
;	out:	none
;------------------------------------------------------------

gouraud_triangle proc

@@x1 equ word ptr [bp+4]
@@y1 equ word ptr [bp+6]
@@x2 equ word ptr [bp+8]
@@y2 equ word ptr [bp+10]
@@x3 equ word ptr [bp+12]
@@y3 equ word ptr [bp+14]
@@col1 equ word ptr [bp+16]
@@col2 equ word ptr [bp+18]
@@col3 equ word ptr [bp+20]

@@dx12 equ dword ptr [bp-4]
@@dx13 equ dword ptr [bp-8]
@@dx23 equ dword ptr [bp-12]

@@dc12 equ dword ptr [bp-16]
@@dc13 equ dword ptr [bp-20]
@@dc23 equ dword ptr [bp-24]
@@c1 equ dword ptr [bp-28]
@@c2 equ dword ptr [bp-32]

	push	bp
	mov	bp, sp
	sub	sp, 32

	push	es
	mov	es, buffer_seg

	mov	ax, @@y1
	cmp	ax, @@y3
	jl	gt_check1

	xchg	ax, @@y3
	mov	@@y1, ax

	mov	ax, @@x1
	xchg	ax, @@x3
	mov	@@x1, ax

	mov	ax, @@col1
	xchg	ax, @@col3
	mov	@@col1, ax
gt_check1:
	mov	ax, @@y2
	cmp	ax, @@y3
	jl	gt_check2

	xchg	ax, @@y3
	mov	@@y2, ax

	mov	ax, @@x2
	xchg	ax, @@x3
	mov	@@x2, ax

	mov	ax, @@col2
	xchg	ax, @@col3
	mov	@@col2, ax
gt_check2:
	mov	ax, @@y1
	cmp	ax, @@y2
	jl	gt_check3

	xchg	ax, @@y2
	mov	@@y1, ax

	mov	ax, @@x1
	xchg	ax, @@x2
	mov	@@x1, ax

	mov	ax, @@col1
	xchg	ax, @@col2
	mov	@@col1, ax
gt_check3:


	mov	bx, @@y2
	sub	bx, @@y1
	jnz	gt_dx12_make

	mov	@@dx12, 0
	mov	@@dc12, 0
	jmp	gt_dx12_done
gt_dx12_make:
	mov	ax, @@x2
	sub	ax, @@x1
	movsx	eax, ax
	movsx	ebx, bx
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dx12, eax			; dx12 = (x2-x1)/(y2-y1)

	mov	ax, @@col2
	sub	ax, @@col1
	movsx	eax, ax
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dc12, eax			; dc12 = (col2-col1)/(y2-y1)
gt_dx12_done:

	mov	bx, @@y3
	sub	bx, @@y1
	jnz	gt_dx13_make

	mov	@@dx13, 0
	mov	@@dc13, 0
	jmp	gt_dx13_done
gt_dx13_make:
	mov	ax, @@x3
	sub	ax, @@x1
	movsx	eax, ax
	movsx	ebx, bx
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dx13, eax			; dx13 = (x3-x1)/(y3-y1)

	mov	ax, @@col3
	sub	ax, @@col1
	movsx	eax, ax
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dc13, eax			; dc13 = (col3-col1)/(y3-y1)
gt_dx13_done:

	mov	bx, @@y3
	sub	bx, @@y2
	jnz	gt_dx23_make

	mov	@@dx23, 0
	mov	@@dc23, 0
	jmp	gt_dx23_done
gt_dx23_make:
	mov	ax, @@x3
	sub	ax, @@x2
	movsx	eax, ax
	movsx	ebx, bx
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dx23, eax			; dx23 = (x3-x2)/(y3-y2)

	mov	ax, @@col3
	sub	ax, @@col2
	movsx	eax, ax
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	@@dc23, eax			; dc23 = (col3-col2)/(y3-y2)
gt_dx23_done:


	movsx	eax, @@x1
	shl	eax, SHIFT_CONST
	mov	ebx, eax
	movsx	edx, @@col1
	shl	edx, SHIFT_CONST
	mov	@@c1, edx
	mov	@@c2, edx
	mov	cx, @@y1
	cmp	cx, @@y2
	jge	gt_loop1_end
gt_loop1:
	push	eax
	push	ebx
	push	cx

	mov	edx, @@c2
	sar	edx, SHIFT_CONST
	push	dx
	mov	edx, @@c1
	sar	edx, SHIFT_CONST
	push	dx
	push	cx
	sar	ebx, SHIFT_CONST
	push	bx
	sar	eax, SHIFT_CONST
	push	ax
	call	gouraud_line

	pop	cx
	pop	ebx
	pop	eax

	mov	edx, @@dc13
	add	@@c1, edx
	mov	edx, @@dc12
	add	@@c2, edx

	add	eax, @@dx13
	add	ebx, @@dx12
	inc	cx
	cmp	cx, @@y2
	jl	gt_loop1
gt_loop1_end:
	
	mov	cx, @@y2
	cmp	cx, @@y3
	jge	gt_loop2_end
	movsx	ebx, @@x2
	shl	ebx, SHIFT_CONST
	movsx	edx, @@col2
	shl	edx, SHIFT_CONST
	mov	@@c2, edx
gt_loop2:
	push	eax
	push	ebx
	push	cx

	mov	edx, @@c2
	sar	edx, SHIFT_CONST
	push	dx
	mov	edx, @@c1
	sar	edx, SHIFT_CONST
	push	dx
	push	cx
	sar	ebx, SHIFT_CONST
	push	bx
	sar	eax, SHIFT_CONST
	push	ax
	call	gouraud_line

	pop	cx
	pop	ebx
	pop	eax

	mov	edx, @@dc13
	add	@@c1, edx
	mov	edx, @@dc23
	add	@@c2, edx

	add	eax, @@dx13
	add	ebx, @@dx23
	inc	cx
	cmp	cx, @@y3
	jl	gt_loop2
gt_loop2_end:

	pop	es

	mov	sp, bp
	pop	bp
	ret	18
endp

;------------------------------------------------------------
;	in:	x1, x2, y, c1, c2
;	out:	none
;------------------------------------------------------------

gouraud_line proc

@@x1 equ word ptr [bp+4]
@@x2 equ word ptr [bp+6]
@@y equ word ptr [bp+8]
@@col1 equ word ptr [bp+10]
@@col2 equ word ptr [bp+12]

	push	bp
	mov	bp, sp

	mov	ax, @@y
	or	ax, ax
	jl	gl_quit
	cmp	ax, 199
	jg	gl_quit

	mov	ax, @@x1
	cmp	ax, @@x2
	je	gl_quit
	jl	gl_ok

	xchg	ax, @@x2
	mov	@@x1, ax

	mov	ax, @@col1
	xchg	ax, @@col2
	mov	@@col1, ax
gl_ok:
	mov	di, @@y
	mov	ax, di
	shl	di, 6
	shl	ax, 8
	add	di, ax
	add	di, @@x1

	mov	ax, @@col2
	sub	ax, @@col1
	movsx	eax, ax
	shl	eax, SHIFT_CONST
	cdq
	mov	cx, @@x2
	sub	cx, @@x1
	movsx	ecx, cx
	idiv	ecx
	mov	edx, eax			; edx - delta_c

	movsx	ebx, @@col1
	shl	ebx, SHIFT_CONST		; ebx - curr_c
gl_draw:
	mov	eax, ebx
	sar	eax, SHIFT_CONST
	stosb
	add	ebx, edx
	dec	ecx
	jnz	gl_draw
gl_quit:
	pop	bp
	ret	10
endp