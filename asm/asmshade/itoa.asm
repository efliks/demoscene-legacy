.code

;------------------------------------------------------------
;	in:	ax - value to convert
;		di - offset to text buffer
;	out:	none
;------------------------------------------------------------

itoa proc
	mov	bx, 10
	xor	cx, cx
i_con:
	xor	dx, dx
	div	bx
	push	dx
	inc	cx
	or	ax, ax
	jnz	i_con
i_make:
	pop	ax
	add	al, '0'
	stosb
	dec	cx
	jnz	i_make
	xor	al, al
	stosb
	ret
endp