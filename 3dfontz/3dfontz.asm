;------------------------------------------------------------
;	3dFontz 0.7 by Majuma/NAAG
;	Mail: mfelix@polbox.com
;	Web: www.ps.nq.pl / www.naag.prv.pl
;
;	Updated: 08/02/02, 16/02/02, 22/03/02
;------------------------------------------------------------

.386p
locals

code32 segment para public use32
	assume cs:code32, ds:code32

_main:
	finit

	mov	eax, REQ_MEM
	call	alloc_mem
	or	eax, eax
	jz	QuitProgy
	mov	d [MemHandle], eax

	; set pointers
	mov	d [FrameBuffer], ebx
	add	ebx, 64000
	mov	d [SinCosLookups], ebx
	add	ebx, SINCOS_SIZE
	mov	d [VGAPalette], ebx
	add	ebx, 768
	mov	d [BackTex], ebx
	add	ebx, 64000

	call	init_sincos
	call	GetFontAddr

	mov	edi, d [BackTex]
	;call	GenerateSineTexture
	call	MakeTex


;------------------------------------------------------------
;	generate 3d letters
	
	;call	EnableBuffer3d
	;or	eax, eax
	;jnz	EB3d_allocated
	;jmp	FreeAll
EB3d_allocated:
	mov	esi, o HandlersTable
	mov	edi, o Structs3dTable
	mov	ebx, o LettersTable
	mov	ecx, LETTERTAB_LEN
GenerateLetter_loop:
	movzx	eax, b [ebx]
	call	MakeLetter3d

	;call	SaveLetter3d
	or	eax, eax
	jnz	GL_next

	; error -> not enough memory
	;call	DisableBuffer3d
	jmp	FreeAll
GL_next:
	inc	d [LoadedStr3d]
	inc	ebx
	add	esi, 4
	add	edi, size struct3d
	loop	GenerateLetter_loop

	;call	DisableBuffer3d

;------------------------------------------------------------

	mov	edi, d [VGAPalette]
	mov	esi, o phongpal
	mov	ecx, 64 * 3
	cld
	rep	movsb
	
	add	edi, (128 - 64) * 3
	mov	ecx, 128
	xor	eax, eax
InitPal:
	stosb
	stosb
	stosb
	mov	al, ah
	shr	al, 2
	inc	ah
	loop	InitPal

	call	set_13h

	mov	esi, d [VGAPalette]
	call	set_palette


	mov	esi, o Move1
	mov	edi, o StartX
	mov	ecx, (size move3d_struct) / 4
	cld
	rep	movsd

	mov	esi, d [CurrLetter]
	call	InitMovement

;------------------------------------------------------------

MainLoop:
	dec	d [TimeToChange]
	jnz	DoNotChangeLetter
	mov	d [TimeToChange], LETTER_TIME

	add	d [CurrLetter], size struct3d
	inc	d [CurrLetterCnt]
	cmp	d [CurrLetterCnt], LETTERTAB_LEN
	jne	StartLetter

	mov	d [CurrLetter], o Structs3dTable
	mov	d [CurrLetterCnt], 0
StartLetter:
	mov	esi, o Move1
	mov	edi, o StartX
	mov	ecx, (size move3d_struct) / 4
	cld
	rep	movsd

	mov	esi, d [CurrLetter]
	call	InitMovement

	jmp	DoNotChangeMovement
DoNotChangeLetter:

	cmp	d [TimeToChange], LETTER_TIME / 2
	jne	DoNotChangeMovement

	mov	esi, o Move2
	mov	edi, o StartX
	mov	ecx, (size move3d_struct) / 4
	cld
	rep	movsd

	mov	esi, d [CurrLetter]
	call	InitMovement
DoNotChangeMovement:


	mov	esi, d [BackTex]
	mov	edi, d [FrameBuffer]
	mov	ecx, 16000
	cld
	rep	movsd

	mov	esi, d [CurrLetter]
	call	MoveStruct3d

	mov	esi, d [CurrLetter]
	call	DrawGouraudFaces


	mov	esi, d [CurrLetter]
	lea	esi, d [esi.s3d_AngleX]
	add	d [esi + 0], 1
	add	d [esi + 4], 3
	add	d [esi + 8], 1

	call	PrintFPS

	mov	esi, o txt_app
	mov	ecx, 160 - (TXTAPP_LEN * 4)
	mov	edx, 199 - 8
	mov	eax, TEXT_COLOR
	call	put_string

	; DEBUG !
comment #
	mov	eax, d [TotalAllocMem]
	mov	edi, o test_txt
	call	itoa
	mov	esi, o test_txt
	xor	ecx, ecx
	mov	edx, 12
	mov	eax, TEXT_COLOR
	call	put_string #

	call	wait_vr
	call	copy_buffer

	in	al, 60h
	dec	al
	jnz	MainLoop

;------------------------------------------------------------

	call	set_03h

FreeAll:
	mov	eax, d [MemHandle]
	call	free_mem

FreeStructs3d:
	mov	esi, o HandlersTable
	mov	ecx, d [LoadedStr3d]
	or	ecx, ecx
	jz	QuitProgy
	cld
FS3d_loop:
	lodsd
	push	esi
	push	ecx
	call	free_mem
	pop	ecx
	pop	esi
	loop	FS3d_loop

QuitProgy:
	ret

;------------------------------------------------------------

include kernel\kernel.inc
include 3dfontz.inc

include alloc.asm
include graph.asm
include font.asm
include grd_tri.asm
include shdline.asm
include letgen.asm
include 3dmath.asm
include engine.asm
include itoa.asm
include fps.asm
;include blurer.asm
;include sintex.asm
include move3d.asm
include mktex.asm
include phongpal.inc

MemHandle	dd ?
VGAPalette	dd ?
BackTex		dd ?

LettersTable	db '@ABC123'
LETTERTAB_LEN	equ $ - LettersTable

HandlersTable	dd LETTERTAB_LEN dup(?)
Structs3dTable	struct3d LETTERTAB_LEN dup(?)	
LoadedStr3d	dd 0

CurrLetter	dd o Structs3dTable
CurrLetterCnt	dd 0
TimeToChange	dd LETTER_TIME

Move1		move3d_struct <0.0, 0.0, -180.0, 0.0, 0.0, 0.0>
Move2		move3d_struct <0.0, 0.0, 0.0, 160.0, -100.0, -100.0>

txt_app		db '3dFontz 0.7 test by Majuma/NAAG',0
TXTAPP_LEN	equ $ - txt_app - 1

test_txt	db 8 dup(?)

code32 ends
end
