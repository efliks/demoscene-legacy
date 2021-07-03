.386p
locals

b	equ byte ptr
w	equ word ptr
d	equ dword ptr
o	equ offset

SHIFT_CONST equ 12

code32 segment para public use32
	assume cs:code32, ds:code32

;------------------------------------------------------------
;	in:	filled t_strc
;	out:	none
;------------------------------------------------------------

bump_triangle proc

	mov	eax, t_y1
	cmp	eax, t_y3
	jle	@@ok13

	xchg	eax, t_y3
	mov	t_y1, eax

	mov	eax, t_x1
	xchg	eax, t_x3
	mov	t_x1, eax


	mov	eax, b_y1
	xchg	eax, b_y3
	mov	b_y1, eax

	mov	eax, b_x1
	xchg	eax, b_x3
	mov	b_x1, eax

	mov	eax, e_y1
	xchg	eax, e_y3
	mov	e_y1, eax

	mov	eax, e_x1
	xchg	eax, e_x3
	mov	e_x1, eax
@@ok13:

	mov	eax, t_y2
	cmp	eax, t_y3
	jle	@@ok23

	xchg	eax, t_y3
	mov	t_y2, eax

	mov	eax, t_x2
	xchg	eax, t_x3
	mov	t_x2, eax


	mov	eax, b_y2
	xchg	eax, b_y3
	mov	b_y2, eax

	mov	eax, b_x2
	xchg	eax, b_x3
	mov	b_x2, eax

	mov	eax, e_y2
	xchg	eax, e_y3
	mov	e_y2, eax

	mov	eax, e_x2
	xchg	eax, e_x3
	mov	e_x2, eax
@@ok23:

	mov	eax, t_y1
	cmp	eax, t_y2
	jle	@@ok12

	xchg	eax, t_y2
	mov	t_y1, eax

	mov	eax, t_x1
	xchg	eax, t_x2
	mov	t_x1, eax


	mov	eax, b_y1
	xchg	eax, b_y2
	mov	b_y1, eax

	mov	eax, b_x1
	xchg	eax, b_x2
	mov	b_x1, eax

	mov	eax, e_y1
	xchg	eax, e_y2
	mov	e_y1, eax

	mov	eax, e_x1
	xchg	eax, e_x2
	mov	e_x1, eax
@@ok12:
	

	mov	ebx, t_y2
	sub	ebx, t_y1
	jnz	@@make_d12

	mov	dx12, 0

	mov	dbx12, 0
	mov	dby12, 0
	mov	dex12, 0
	mov	dey12, 0

	jmp	@@done_d12
@@make_d12:
	mov	eax, t_x2
	sub	eax, t_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dx12, eax

	mov	eax, b_x2
	sub	eax, b_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dbx12, eax

	mov	eax, b_y2
	sub	eax, b_y1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dby12, eax

	mov	eax, e_x2
	sub	eax, e_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dex12, eax

	mov	eax, e_y2
	sub	eax, e_y1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dey12, eax
@@done_d12:


	mov	ebx, t_y3
	sub	ebx, t_y1
	jnz	@@make_d13

	mov	dx13, 0

	mov	dbx13, 0
	mov	dby13, 0
	mov	dex13, 0
	mov	dey13, 0

	jmp	@@done_d13
@@make_d13:
	mov	eax, t_x3
	sub	eax, t_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dx13, eax

	mov	eax, b_x3
	sub	eax, b_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dbx13, eax

	mov	eax, b_y3
	sub	eax, b_y1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dby13, eax

	mov	eax, e_x3
	sub	eax, e_x1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dex13, eax

	mov	eax, e_y3
	sub	eax, e_y1
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dey13, eax
@@done_d13:


	mov	ebx, t_y3
	sub	ebx, t_y2
	jnz	@@make_d23

	mov	dx23, 0

	mov	dbx23, 0
	mov	dby23, 0
	mov	dex23, 0
	mov	dey23, 0

	jmp	@@done_d23
@@make_d23:
	mov	eax, t_x3
	sub	eax, t_x2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dx23, eax

	mov	eax, b_x3
	sub	eax, b_x2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dbx23, eax

	mov	eax, b_y3
	sub	eax, b_y2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dby23, eax

	mov	eax, e_x3
	sub	eax, e_x2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dex23, eax

	mov	eax, e_y3
	sub	eax, e_y2
	shl	eax, SHIFT_CONST
	cdq
	idiv	ebx
	mov	dey23, eax
@@done_d23:


	mov	eax, t_x1
	shl	eax, SHIFT_CONST
	mov	cx1, eax
	mov	cx2, eax
	
	mov	eax, b_x1
	shl	eax, SHIFT_CONST
	mov	cbx1, eax
	mov	cbx2, eax

	mov	eax, b_y1
	shl	eax, SHIFT_CONST
	mov	cby1, eax
	mov	cby2, eax

	mov	eax, e_x1
	shl	eax, SHIFT_CONST
	mov	cex1, eax
	mov	cex2, eax

	mov	eax, e_y1
	shl	eax, SHIFT_CONST
	mov	cey1, eax
	mov	cey2, eax

	mov	ecx, t_y1
	cmp	ecx, t_y2
	jge	@@loop12_done
@@loop12:
	call	_call_bump_line

	mov	eax, dx13
	add	cx1, eax
	mov	eax, dx12
	add	cx2, eax

	mov	eax, dbx13
	add	cbx1, eax
	mov	eax, dbx12
	add	cbx2, eax
	mov	eax, dby13
	add	cby1, eax
	mov	eax, dby12
	add	cby2, eax

	mov	eax, dex13
	add	cex1, eax
	mov	eax, dex12
	add	cex2, eax
	mov	eax, dey13
	add	cey1, eax
	mov	eax, dey12
	add	cey2, eax

	inc	ecx
	cmp	ecx, t_y2
	jl	@@loop12
@@loop12_done:

	
	mov	eax, t_x2
	shl	eax, SHIFT_CONST
	mov	cx2, eax
	
	mov	eax, b_x2
	shl	eax, SHIFT_CONST
	mov	cbx2, eax

	mov	eax, b_y2
	shl	eax, SHIFT_CONST
	mov	cby2, eax

	mov	eax, e_x2
	shl	eax, SHIFT_CONST
	mov	cex2, eax

	mov	eax, e_y2
	shl	eax, SHIFT_CONST
	mov	cey2, eax

	mov	ecx, t_y2
	cmp	ecx, t_y3
	jge	@@loop23_done
@@loop23:
	call	_call_bump_line

	mov	eax, dx13
	add	cx1, eax
	mov	eax, dx23
	add	cx2, eax

	mov	eax, dbx13
	add	cbx1, eax
	mov	eax, dbx23
	add	cbx2, eax
	mov	eax, dby13
	add	cby1, eax
	mov	eax, dby23
	add	cby2, eax

	mov	eax, dex13
	add	cex1, eax
	mov	eax, dex23
	add	cex2, eax
	mov	eax, dey13
	add	cey1, eax
	mov	eax, dey23
	add	cey2, eax

	inc	ecx
	cmp	ecx, t_y3
	jl	@@loop23
@@loop23_done:

	ret
endp

_call_bump_line proc
	push	ecx

	push	d frame_buffer
	push	d t_emap
	push	d t_bmap

	push	d cey2
	push	d cex2
	push	d cey1
	push	d cex1

	push	d cby2
	push	d cbx2
	push	d cby1
	push	d cbx1

	push	ecx

	mov	eax, cx2
	sar	eax, SHIFT_CONST
	push	eax

	mov	eax, cx1
	sar	eax, SHIFT_CONST
	push	eax

	call	bump_line
	pop	ecx
	ret
endp

dx12	dd ?
dx13	dd ?
dx23	dd ?
cx1	dd ?
cx2	dd ?

dbx12	dd ?
dby12	dd ?
dbx13	dd ?
dby13	dd ?
dbx23	dd ?
dby23	dd ?
cbx1	dd ?
cby1	dd ?
cbx2	dd ?
cby2	dd ?

dex12	dd ?
dey12	dd ?
dex13	dd ?
dey13	dd ?
dex23	dd ?
dey23	dd ?
cex1	dd ?
cey1	dd ?
cex2	dd ?
cey2	dd ?

t_strc:
t_x1	dd ?
t_y1	dd ?
t_x2	dd ?
t_y2	dd ?
t_x3	dd ?
t_y3	dd ?

b_x1	dd ?
b_y1	dd ?
b_x2	dd ?
b_y2	dd ?
b_x3	dd ?
b_y3	dd ?

e_x1	dd ?
e_y1	dd ?
e_x2	dd ?
e_y2	dd ?
e_x3	dd ?
e_y3	dd ?

t_bmap	dd ?
t_emap	dd ?

code32 ends