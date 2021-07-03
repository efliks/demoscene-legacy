.386p
locals

b	equ byte ptr
w	equ word ptr
d	equ dword ptr
o	equ offset

SHIFT_CONST equ 12

code32 segment para public use32
	assume cs:code32, ds:code32

bump_line proc

@@x1	equ d [ebp + 08]
@@x2	equ d [ebp + 12]
@@y	equ d [ebp + 16]

; fixed point bumpmap coords
@@bx1	equ d [ebp + 20]
@@by1	equ d [ebp + 24]
@@bx2	equ d [ebp + 28]
@@by2	equ d [ebp + 32]

; fixed point envmap coords
@@ex1	equ d [ebp + 36]
@@ey1	equ d [ebp + 40]
@@ex2	equ d [ebp + 44]
@@ey2	equ d [ebp + 48]

@@bmap	equ d [ebp + 52]
@@emap	equ d [ebp + 56]
@@buff	equ d [ebp + 60]


@@dbx	equ d [ebp - 04]
@@dby	equ d [ebp - 08]
@@dex	equ d [ebp - 12]
@@dey	equ d [ebp - 16]

@@cbx	equ d [ebp - 20]
@@cby	equ d [ebp - 24]
@@cex	equ d [ebp - 28]
@@cey	equ d [ebp - 32]

	push	ebp
	mov	ebp, esp
	sub	esp, 32

	mov	eax, @@y
	or	eax, eax
	jl	@@quit
	cmp	eax, 199
	jg	@@quit

	mov	eax, @@x1
	cmp	eax, @@x2
	jl	@@ok
	je	@@quit

	xchg	eax, @@x2
	mov	@@x1, eax

	mov	eax, @@bx1
	xchg	eax, @@bx2
	mov	@@bx1, eax

	mov	eax, @@by1
	xchg	eax, @@by2
	mov	@@by1, eax

	mov	eax, @@ex1
	xchg	eax, @@ex2
	mov	@@ex1, eax

	mov	eax, @@ey1
	xchg	eax, @@ey2
	mov	@@ey1, eax
@@ok:
	mov	edi, @@y
	mov	eax, edi
	shl	edi, 6
	shl	eax, 8
	add	edi, eax
	add	edi, @@x1
	add	edi, frame_buffer

	mov	ecx, @@x2
	sub	ecx, @@x1


	mov	eax, @@bx2
	sub	eax, @@bx1
	cdq
	idiv	ecx
	mov	@@dbx, eax

	mov	eax, @@by2
	sub	eax, @@by1
	cdq
	idiv	ecx
	mov	@@dby, eax

	mov	eax, @@ex2
	sub	eax, @@ex1
	cdq
	idiv	ecx
	mov	@@dex, eax

	mov	eax, @@ey2
	sub	eax, @@ey1
	cdq
	idiv	ecx
	mov	@@dey, eax


	mov	eax, @@bx1
	mov	@@cbx, eax
	mov	eax, @@by1
	mov	@@cby, eax

	mov	eax, @@ex1
	mov	@@cex, eax
	mov	eax, @@ey1
	mov	@@cey, eax
@@draw:
	mov	eax, @@cby
	sar	eax, SHIFT_CONST
	shl	eax, 7
	mov	esi, @@cbx
	sar	esi, SHIFT_CONST
	add	esi, eax

	
	mov	ebx, esi
	dec	ebx
	and	ebx, 16383
	add	ebx, @@bmap
	movzx	eax, b [ebx]

	mov	ebx, esi
	inc	ebx
	and	ebx, 16383
	add	ebx, @@bmap
	movzx	ebx, b [ebx]
	sub	eax, ebx

	mov	ebx, esi
	sub	ebx, 128
	and	ebx, 16383
	add	ebx, @@bmap
	movzx	edx, b [ebx]

	mov	ebx, esi
	add	ebx, 128
	and	ebx, 16383
	add	ebx, @@bmap
	movzx	ebx, b [ebx]
	sub	edx, ebx


	mov	ebx, @@cex
	sar	ebx, SHIFT_CONST
	add	eax, ebx

	mov	ebx, @@cey
	sar	ebx, SHIFT_CONST
	add	edx, ebx


	or	eax, eax
	jl	@@black
	cmp	eax, 127
	jg	@@black
	or	edx, edx
	jl	@@black
	cmp	edx, 127
	jg	@@black

	shl	edx, 7
	add	edx, eax
	add	edx, @@emap
	mov	al, b [edx]

	jmp	@@put_pixel
@@black:
	xor	eax, eax
@@put_pixel:
	stosb

	mov	eax, @@dbx
	add	@@cbx, eax
	mov	eax, @@dby
	add	@@cby, eax

	mov	eax, @@dex
	add	@@cex, eax
	mov	eax, @@dey
	add	@@cey, eax

	dec	ecx
	jnz	@@draw
@@quit:
	mov	esp, ebp
	pop	ebp
	ret	14 * 4
endp

code32 ends