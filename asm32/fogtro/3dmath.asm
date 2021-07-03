.386p
locals

code32 segment para public use32
	assume cs:code32,  ds:code32

init_sincos proc
	finit
	fldz

	xor	edi, edi
	mov	ecx, MAX_DEGS
@@make:
	fld	st
	fld	st
	fsin
	fstp	dword ptr sintab[edi]
	fcos
	fstp	dword ptr costab[edi]
	fadd	delta_angle
	add	edi, 4
	dec	ecx
	jnz	@@make

	ffree	st
	ret
endp


; pi/(MAX_DEGS/2)
delta_angle	dd 0.0122718463031

sintab		dd MAX_DEGS dup(?)
costab		dd MAX_DEGS dup(?)


code32 ends