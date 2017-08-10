
;************************************************************
;	FlashScreen()
;
;	in: esi = ptr to base palette
;************************************************************
FlashScreen:
	mov	ebp, esi	; ebp = ptr to base palette
	sub	esp, 768
	mov	ebx, esp	; ebx = ptr to current palette

	mov	al, b [FaderFlag]
	or	al, al
	jz	FS_quit

	mov	al, b [FaderCounter]
	inc	al
	cmp	al, FLASH_TIME
	jb	FS_cnt_done
	xor	al, al
	mov	dl, b [FaderFlag]
	inc	dl
	and	dl, 3
	mov	b [FaderFlag], dl
FS_cnt_done:
	mov	b [FaderCounter], al

	mov	edi, ebx
	call	get_palette

	mov	al, b [FaderFlag]
	cmp	al, 1
	je	FS_inc
	
	xor	esi, esi
	mov	ecx, 768
FS_dec_do:
	mov	al, b [ebx + esi]
	cmp	al, b [ebp + esi]
	jbe	FS_dec_skip
	dec	al
	mov	b [ebx + esi], al
FS_dec_skip:
	inc	esi
	dec	ecx
	jnz	FS_dec_do

	jmp	FS_set

FS_inc:
	xor	esi, esi
	mov	ecx, 768
FS_inc_do:
	mov	al, b [ebx + esi]
	cmp	al, 63
	je	FS_inc_skip
	inc	al
	mov	b [ebx + esi], al
FS_inc_skip:
	inc	esi
	dec	ecx
	jnz	FS_inc_do

FS_set:
	mov	esi, ebx
	call	set_palette
FS_quit:
	add	esp, 768
	ret

;************************************************************

FaderCounter	db 0
FaderFlag	db 0
