
; in: edi = ptr to 320x200 texture
MakeTex:
	xor	edx, edx
mt_v:
	xor	ecx, ecx
mt_h:
	push	edx
	push	ecx

	mov	eax, ecx
	xor	eax, edx
	and	eax, 15
	mov	ebx, eax

	mov	eax, edx
	sub	eax, 100
	imul	eax
	push	eax

	mov	eax, ecx
	sub	eax, 160
	imul	eax
	shr	eax, 1

	pop	edx
	add	eax, edx
	mov	d [mt_temp32], eax
	fild	d [mt_temp32]
	fsqrt
	fistp	d [mt_temp32]
	
	mov	eax, 127
	sub	eax, d [mt_temp32]
	add	eax, ebx
	or	eax, eax
	jge	mt_ok1
	xor	eax, eax
	jmp	mt_ok2
mt_ok1:
	cmp	eax, 127
	jle	mt_ok2
	mov	eax, 127
mt_ok2:
	add	eax, 128
	stosb

	pop	ecx
	pop	edx
	inc	ecx
	cmp	ecx, 320
	jne	mt_h

	inc	edx
	cmp	edx, 200
	jne	mt_v
	ret

mt_temp32	dd ?
