.386p
locals

code32 segment para public use32
	assume cs:code32,  ds:code32

init_font proc
	push	ebp
	mov	ebp, esp
	sub	esp, size dpmi_regs

	mov	edi, esp
	mov	dword ptr [edi._eax], 00001130h
	mov	dword ptr [edi._ebx], 00000300h
	mov	eax, 0300h
	mov	ebx, 10h
	int	31h

	movzx	eax, word ptr [edi._es]
	shl	eax, 4
	movzx	ebx, word ptr [edi._ebp]
	add	eax, ebx
	sub	eax, code32_base
	mov	font_addr, eax

	mov	esp, ebp
	pop	ebp
	ret
endp

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
	add	edi, frame_buffer

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
	add	ebx, font_addr

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

font_addr	dd ?
_sx		dd ?
_sy		dd ?
_sc		db ?

code32 ends