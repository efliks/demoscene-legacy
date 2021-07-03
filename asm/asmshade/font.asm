.data?
	font_data db 2048 dup(?)

.code

init_font proc
	push	bp
	push	ds
	push	es
	
	mov	ax, 01130h
	mov	bh, 03h
	int	10h
	
	mov	ax, es
	mov	ds, ax
	mov	si, bp

	mov	ax, cs
	mov	es, ax
	mov	di, offset font_data

	mov	cx, 2048/4
	cld
	rep	movsd

	pop	es
	pop	ds
	pop	bp
	ret
endp

;------------------------------------------------------------
;	in:	si - offset to null-terminated string
;		al - color
;		cx - x
;		dx - y
;	out:	none
;------------------------------------------------------------

put_string proc
	push	es
	mov	es, buffer_seg

	mov	di, dx
	shl	di, 6
	shl	dx, 8
	add	di, dx
	add	di, cx

	mov	ah, al
ps_char:
	lodsb
	or	al, al
	jz	ps_quit

	; draw letter
	push	di
	mov	dl, ah
	movzx	bx, al
	shl	bx, 3
	add	bx, offset font_data

	mov	ch, 8
ps_hor:
	mov	cl, 8
	mov	al, byte ptr [bx]
ps_ver:
	rcl	al, 1
	jae	ps_next
	mov	byte ptr es:[di], dl
ps_next:
	inc	di
	dec	cl
	jnz	ps_ver
	inc	bx
	add	di, 312
	inc	dl
	dec	ch
	jnz	ps_hor
	pop	di

	; next letter
	add	di, 8
	jmp	ps_char
ps_quit:
	pop	es
	ret
endp