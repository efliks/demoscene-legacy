.386p
locals

code32 segment para public use32
	assume cs:code32, ds:code32

;------------------------------------------------------------
;	in:	eax - value to convert
;		edi - offset to text buffer
;	out:	none
;------------------------------------------------------------

itoa proc
	mov	ebx, 10
	xor	ecx, ecx
@@con:
	xor	edx, edx
	div	ebx
	push	edx
	inc	ecx
	or	eax, eax
	jnz	@@con
@@make:
	pop	eax
	add	al, '0'
	stosb
	dec	ecx
	jnz	@@make
	xor	eax, eax
	stosb
	ret
endp

code32 ends
