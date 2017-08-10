
; out: eax = 1 if new second, otherwise eax = 0
GetSec:
	xor	eax, eax
	out	70h, al
	in	al, 71h
	cmp	al, b [LastSec]
	jne	GSec_changed
	xor	eax ,eax
	ret
GSec_changed:
	mov	b [LastSec], al
	mov	eax, 1
	ret

;------------------------------------------------------------

PrintFPS:
	mov	esi, o txt_framerate
	xor	ecx, ecx
	xor	edx, edx
	mov	eax, TEXT_COLOR
	call	put_string

	inc	d [FrameCounter]

	call	GetSec
	or	eax, eax
	jz	SkipCalcFrame

	mov	eax, d [FrameCounter]
	mov	edi, o txt_framerate + 5
	call	itoa
	mov	d [FrameCounter], 0
SkipCalcFrame:
	ret

;------------------------------------------------------------

FrameCounter	dd 0
LastSec		db 0
txt_framerate	db 'FPS: ', 4 dup(0)
