
;************************************************************
; Fogtro! v.1.0
;
; Idea and coding by Majuma. All rights reserved to Majuma.
; Feel free to copy and distribute this code as you will
; but do not change it without my permission !
; Have any suggestions? Mail me -> majuma@wp.pl
;
; Build date: 17/08/03
;
; ASM not dead ! :)
;************************************************************

.386p
locals

code32 segment para public use32
	assume cs:code32,  ds:code32

;************************************************************
;	entry point
;************************************************************

_main:
	; analize command line !
	mov	esi, d [PSP_addr]
	add	esi, 80h
	push	esi
	push	esi
	mov	edi, o cmd_Speed
	call	FindCommand
	or	eax, eax
	jz	acl_NoFullSpeed
	mov	b [FullSpeedFlag], 1
acl_NoFullSpeed:
	pop	esi
	mov	edi, o cmd_FogZ
	call	FindCommand
	or	eax, eax
	jz	acl_NoFogz
	mov	d [CurrColorProc], o GetVertexColorZ
acl_NoFogZ:
	pop	esi
	mov	edi, o cmd_Flat
	call	FindCommand
	or	eax, eax
	jz	acl_NoFlat
	mov	b [FlatFlag], 1
acl_NoFlat:


	finit

	call	GetFontAddr

	; compute requied memory
	call	World1_GetMem
	push	eax
	call	World2_GetMem
	pop	ebx
	add	eax, ebx
	push	eax
	call	World3_GetMem
	pop	ebx
	add	eax, ebx

	; alloc memory
	add	eax, REQ_MEM
	call	alloc_mem
	or	eax, eax
	jz	QuitProg
	mov	d [MemHandle], eax
	mov	eax, ebx

	; set pointers
	mov	d [FrameBuffer], eax
	add	eax, 64000

	mov	d [SmallScrPtr], eax
	add	eax, SMALL_SCR_MEM

	mov	d [PalettePtr], eax
	add	eax, 768

	mov	d [t_TempStrPtr], eax
	push	eax
	mov	edi, eax
	xor	eax, eax
	mov	ecx, MAX_STR_LEN / 4
	cld
	rep	stosd
	pop	eax
	add	eax, MAX_STR_LEN

	call	World1_Init
	call	World2_Init
	call	World3_Init


comment #
	; init palette
	mov	edi, d [PalettePtr]
	mov	ecx, 64
	xor	eax, eax
@@pal1:
	xor	al, al
	stosb
	stosb
	mov	al, ah
	stosb
	inc	ah
	loop	@@pal1

	mov	ecx, 64
	xor	eax, eax
@@pal2:
	mov	al, ah
	stosb
	xor	al, al
	stosb
	stosb
	inc	ah
	loop	@@pal2

	mov	ecx, 64
	xor	eax, eax
@@pal3:
	xor	al, al
	stosb
	mov	al, ah
	stosb
	stosb
	inc	ah
	loop	@@pal3

	mov	ecx, 64
	xor	eax, eax
@@pal4:
	mov	al, ah
	stosb
	stosb
	xor	al, al
	stosb
	inc	ah
	loop	@@pal4 #


	mov	edi, d [PalettePtr]
	mov	esi, o Palette1
	mov	ecx, 64 * 3 / 4
	push	esi
	push	ecx
	cld
	rep	movsd
	pop	ecx
	pop	esi
	rep	movsd
	mov	esi, o Palette2
	mov	ecx, 64 * 3 / 4
	push	esi
	push	ecx
	rep	movsd
	pop	ecx
	pop	esi
	rep	movsd

	mov	edi, d [PalettePtr]
	call	reverse_palette

	mov	edi, d [PalettePtr]
	xor	al, al
	stosb
	stosb
	stosb
	

	call	set_13h

	; set palette
	mov	esi, d [PalettePtr]
	call	set_palette

	

	mov	eax, d [World1_Strc + S3D_N_VERTS]
	mov	edi, o txt_n_verts
	call	itoa

	mov	eax, d [World1_Strc + S3D_N_FACES]
	mov	edi, o txt_n_faces
	call	itoa

	mov	b [FaderFlag], 0


	mov	eax, o World1_Proc
	mov	d [CurrWorld_Proc], eax


;************************************************************
;	main loop
;************************************************************

MainLoop:
	inc	d [_FrameCounter]

	call	clear_buffer
	call	d [CurrWorld_Proc]

	mov	eax, (320 - SMALL_SCR_W) - 4
	mov	ebx, (200 - SMALL_SCR_H) - 4
	call	SmallScr


comment #
	xor	ecx, edx
	xor	edx, edx
	mov	esi, o txt_n_verts
	mov	al, TEXT_COLOR
	call	put_string

	xor	ecx, ecx
	mov	edx, 12
	mov	esi, o txt_n_faces
	mov	al, TEXT_COLOR
	call	put_string #

	
	mov	esi, d [t_TempStrPtr]
	push	esi
	call	GetStrLen
	mov	ecx, 160
	shl	eax, 2
	sub	ecx, eax
	mov	edx, 2
	mov	eax, TEXT_COLOR
	pop	esi
	call	put_string

	mov	al, b [t_Counter]
	dec	al
	jnz	tt_skip

	movzx	eax, b [t_CurrStr]
	dec	eax
	shl	eax, 2
	mov	eax, d [eax + t_OfsTable]
	mov	esi, eax
	push	esi
	movzx	ecx, b [t_CurrLetter]
	add	esi, ecx
	lodsb
	pop	esi
	or	al, al
	jz	tt_change_str
	mov	edi, d [t_TempStrPtr]
	inc	ecx
	cld
	rep	movsb
	mov	al, T_CURSOR
	stosb
	xor	al, al
	stosb
	
	inc	b [t_CurrLetter]
	jmp	tt_oki
tt_change_str:
	dec	b [t_WaitForNext]
	jnz	tt_wait
	mov	al, T_WAIT_TIME
	mov	b [t_WaitForNext], al

	mov	eax, d [_FrameCounter]
	mov	edi, o txt_frames
	call	itoa

	xor	al, al
	mov	b [t_CurrLetter], al
	
	movzx	eax, b [t_CurrStr]
	inc	eax
	cmp	eax, T_NSTRS
	jne	tt_oki1
	mov	al, 1
	mov	b [t_CurrStr], al
	jmp	tt_oki
tt_oki1:
	mov	b [t_CurrStr], al
tt_oki:
	mov	al, T_SPEED
tt_skip:
	mov	b [t_Counter], al
tt_wait:


	mov	eax, d [PartCounter]
	dec	eax
	jnz	PartChangeOK

	; change part !
	mov	eax, d [CurrWorld_Proc]
	cmp	eax, o World1_Proc
	je	SetPartTwo
	cmp	eax, o World2_Proc
	je	SetPartThree

	call	World1_FixFloor ; <-- important !
	mov	eax, o World1_Proc
	jmp	PartChanged
SetPartTwo:
	mov	eax, o World2_Proc
	jmp	PartChanged
SetPartThree:
	mov	eax, o World3_Proc

PartChanged:
	mov	d [CurrWorld_Proc], eax

	mov	eax, PART_TIMEOUT
PartChangeOK:
	cmp	eax, FLASH_TIME
	jne	PartChangeOKKK
	mov	b [FaderFlag], 1
PartChangeOKKK:
	mov	d [PartCounter], eax


	;mov	edi, d [FrameBuffer]
	;call	blurer


	mov	al, b [FullSpeedFlag]
	or	al, al
	jz	WaitVR
	xor	ecx, ecx
	mov	edx, 193
	mov	eax, TEXT_COLOR
	call	PrintFPS
	jmp	CopyBuffer
WaitVR:
	call	wait_vr
CopyBuffer:
	call	copy_buffer

	mov	esi, d [PalettePtr]
	call	FlashScreen

	in	al, 60h
	dec	al
	jnz	MainLoop

;************************************************************
;	main loop ends
;************************************************************

	call	set_03h

	mov	eax, d [MemHandle]
	call	free_mem

QuitProg:
	ret


;************************************************************
;	GetStrLen()
;
;	in: esi = ptr to NULL-terminated string
;	out: eax = lenght
;************************************************************
GetStrLen:
	xor	ecx, ecx
	cld
GSL:
	lodsb
	or	al, al
	jz	GSL_break
	inc	ecx
	jmp	GSL
GSL_break:
	mov	eax, ecx
	ret

;************************************************************

; in: esi = ptr to command line, edi = ptr to ASCIIZ command
; out: eax = 1 if succesful, otherwise eax = 0
FindCommand:
	cld
	lodsb
	or	al, al
	jnz	FC_ok
	xor	eax, eax
	ret
FC_ok:
	movzx	ecx, al
	inc	ecx
	xor	edx, edx
FC_1:
	lodsb
	cmp	al, ' '
	je	FC_s_char
	cmp	al, 0dh
	je	FC_s_char
	jmp	FC_next
FC_s_char:
	or	edx, edx
	jnz	FC_ok2
	mov	edx, -1
	jmp	FC_next
FC_ok2:
	push	ecx
	xor	ecx, ecx
	mov	ebp, esi
	sub	ebp, edx
	dec	ebp
FC_2:
	mov	al, b [ebp + ecx]
	cmp	al, b [edi + ecx]
	jne	FC_failed
	inc	ecx
	cmp	ecx, edx
	jne	FC_2
	pop	ecx
	mov	eax, 1
	ret
FC_failed:
	pop	ecx
	mov	edx, -1
FC_next:
	inc	edx
	loop	FC_1
	xor	eax, eax
	ret

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


;************************************************************
;	includes
;************************************************************

include kernel\kernel.inc
include fogtro!.inc

include alloc.asm
include graph.asm
include 3dmath.inc
include sscr.inc
include sscr.asm
include fps.asm
include font.asm
include fader.inc
include fader.asm
include blurer.asm

include engine\oldeng.inc
include engine\oldeng.asm

include engine\engine.inc
include engine\engine.asm
include engine\flat_tri.inc
include engine\flat_tri.asm
include engine\grd_tri.inc
include engine\grd_tri.asm
include engine\effects.asm

include s3dgen\s3dgen.inc
include s3dgen\s3dgen.asm

include letgen\letgen.inc
include letgen\letgen.asm

include worlds\world1.inc
include worlds\world1.asm
include worlds\world2.inc
include worlds\world2.asm
include worlds\world3.inc
include worlds\world3.asm

include Pal1.inc
include Pal2.inc

;************************************************************
;	global variables
;************************************************************

MemHandle	dd 0
PalettePtr	dd 0

CurrWorld_Proc	dd 0
PartCounter	dd PART_TIMEOUT

txt_n_verts	db 8 dup(0)
txt_n_faces	db 8 dup(0)


_FrameCounter	dd 0
FullSpeedFlag	db 0
FlatFlag	db 0

t_CurrStr	db 1
t_CurrLetter	db 0
t_TempStrPtr	dd 0 ; ptr to allocated memory
t_WaitForNext	db T_WAIT_TIME
t_Counter	db T_SPEED

t_OfsTable	dd o t_Str1, o t_Str2, o t_Str3, o t_Str4, o t_Str5, o t_Str6
		dd o t_Str7, o t_Str8
T_NSTRS		equ (($ - t_OfsTable) / 4) + 1

t_Str1		db 'FogTro!', 0
t_Str2		db 'The Never Finished Foggy Intro', 0
t_Str3		db 'Coded by Majuma/NAAG in pure Assembler', 0
t_Str4		db 'Released on 16th of September 2003', 0
t_Str5		db 'Greetz for all users of NAAG''s forum', 0
t_Str6		db 'See http://www.naag.prv.pl/', 0
t_Str7		db 'A bit old-school, don''t you think? ;)', 0
t_Str8		db 'Rendered frames: '
txt_frames	db 8 dup(0)

cmd_Speed	db '-full', 0
cmd_FogZ	db '-fog', 0
cmd_Flat	db '-flat', 0

code32 ends
end
