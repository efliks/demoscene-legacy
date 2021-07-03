; *** output: ax = allocated segment (0 if error)
alloc_seg:
	mov	ah, 0x4a
	mov	bx, 0x1000
	int	0x21
	mov	ah, 0x48
	mov	bx, 0x1000
	int	0x21
	jnc	alloc_ok
	xor	ax, ax
alloc_ok:
	ret

; *** input: ax = segment
free_seg:
	push	es
	mov	es, ax
	mov	ah, 0x49
	int	0x21
	pop	es
	ret

; *** input: cx = min, dx = max, output: ax = random number
;random:
;	mov	bx, word [rndseed]
;	add	bx, 0x9248
;	ror	bx, 3
;	mov	word [rndseed], bx
;	mov	ax, dx
;	sub	ax, cx
;	mul	bx
;	mov	ax, dx
;	add	ax, cx
;	ret

; *** input: ds:si = ptr to filename, es:di = ptr to target
; output: ax = result (0 if error)
load_file:
	mov	ah, 0x3d
	mov	al, 0
	mov	dx, si
	int	0x21
	jnc	lf_ok
	xor	ax, ax
	ret
lf_ok:
	push	ds
	push	es
	pop	ds
	mov	dx, di
	mov	bx, ax
	mov	cx, -1
	mov	ah, 0x3f
	int	0x21
	pop	ds
	mov	ah, 0x3e
	int	0x21
	mov	ax, 1
	ret

; ***
retrace:
	mov	dx, 0x03da
rc1:
	in	al, dx
	test	al, 8
	jz	rc1
rc2:
	in	al, dx
	test	al, 8
	jnz	rc2
	ret

; *** input: ds:si = ptr to palette
set_palette:
	mov	dx, 0x03c8
	xor	ax, ax
	out	dx, al
	inc	dx
	mov	cx, 768
	cld
	rep	outsb
	ret

;----------------------------------------------------------------------

;rndseed		dw 0x1111

