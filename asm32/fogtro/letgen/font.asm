
GetFontAddr:
	mov	ebp, esp
	sub	esp, size dpmi_regs

	mov	edi, esp
	mov	ecx, (size dpmi_regs) / 4
	xor	eax, eax
	cld
	rep	stosd

	mov	edi, esp
	mov	d [edi._eax], 00001130h
	mov	d [edi._ebx], 00000300h
	mov	eax, 300h
	mov	ebx, 10h
	int	31h

	movzx	eax, w [edi._es]
	shl	eax, 4
	movzx	ebx, w [edi._ebp]
	add	eax, ebx
	sub	eax, d [code32_base]
	mov	d [FontAddress], eax

	mov	esp, ebp
	ret

;------------------------------------------------------------
;	in:	esi - offset to null-terminated string
;		al - color
;		ecx - x
;		edx - y
;	out:	none
;------------------------------------------------------------

put_string proc
	mov	_sx, ecx
	mov	_sy, edx
	mov	_sc, al

@@new_line:
	mov	edi, edx
	shl	edi, 6
	shl	edx, 8
	add	edi, edx
	add	edi, ecx
	add	edi, d [FrameBuffer]

	mov	ah, al
@@char:
	lodsb
	or	al, al
	jz	@@quit
	cmp	al, 10
	jne	@@line_ok

	; next line
	mov	ecx, _sx
	add	_sy, 12
	mov	edx, _sy
	mov	al, _sc
	jmp	@@new_line

@@line_ok:
	; draw letter
	push	edi
	mov	dl, ah
	movzx	ebx, al
	shl	ebx, 3
	add	ebx, d [FontAddress]

	mov	ch, 8
@@hor:
	mov	cl, 8
	mov	al, byte ptr [ebx]
@@ver:
	rcl	al, 1
	jnc	@@next
	mov	byte ptr [edi], dl
@@next:
	inc	edi
	dec	cl
	jnz	@@ver
	inc	ebx
	add	edi, 312
	inc	dl
	dec	ch
	jnz	@@hor
	pop	edi

	; next letter
	add	edi, 8
	jmp	@@char
@@quit:
	ret
endp

;------------------------------------------------------------

_sx		dd ?
_sy		dd ?
_sc		db ?

FontAddress	dd ?
