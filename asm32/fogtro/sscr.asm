
;************************************************************
;	SmallScr()
;
;	in: eax = x, ebx = y
;************************************************************
SmallScr:
	mov	esi, d [FrameBuffer]
	mov	edi, d [SmallScrPtr]

	cld
	mov	edx, SMALL_SCR_H
SScr_y:
	mov	ecx, SMALL_SCR_W
SScr_x:
	movsb
	add	esi, (320 / SMALL_SCR_W) - 1
	dec	ecx
	jnz	SScr_x
	add	esi, (320 * 200 / SMALL_SCR_H) - 320
	dec	edx
	jnz	SScr_y

	mov	edi, ebx
	shl	edi, 6
	shl	ebx, 8
	add	edi, ebx
	add	edi, eax
	add	edi, d [FrameBuffer]

	mov	esi, d [SmallScrPtr]
	mov	edx, SMALL_SCR_H
SScr_yy:
	mov	ecx, SMALL_SCR_W / 4
	rep	movsd
	
comment #
	mov	ecx, SMALL_SCR_W
SScr_xx:
	lodsb
	or	al, al
	jz	SScr_next
	mov	b [edi], al
SScr_next:
	inc	edi
	dec	ecx
	jnz	SScr_xx #
	
	add	edi, 320 - SMALL_SCR_W
	dec	edx
	jnz	SScr_yy
	ret

;************************************************************

SmallScrPtr	dd 0
