;------------------------------------------------------------
;	3d Bump Mapping 2.0ß
;
;	Code: Mikolaj Felix alias Majuma 
;	mfelix@polbox.com
;	www.ps.nq.pl / www.naag.prv.pl
;
;	Updated: 17/12/01
;------------------------------------------------------------

.386p
locals

b	equ byte ptr
w	equ word ptr
d	equ dword ptr
o	equ offset

code32 segment para public use32
	assume cs:code32, ds:code32

_main proc
	finit

	mov	eax, REQ_MEM
	call	alloc_mem
	or	eax, eax
	jz	@@quit_progy
	mov	mem_handle, eax
	
	mov	frame_buffer, ebx
	add	ebx, 64000
	;mov	ptr_background, ebx
	;add	ebx, 64000

	; set 3d structure
	mov	d torus3d.s3d_n_points, MAX_P
	mov	d torus3d.s3d_n_faces, MAX_F

	mov	d torus3d.s3d_points, o points
	
	mov	d torus3d.s3d_r_points, ebx
	add	ebx, MAX_P*(size point3d)

	mov	d torus3d.s3d_t_points, ebx
	add	ebx, MAX_P*(size point2d)

	mov	d torus3d.s3d_faces, o faces

	mov	d torus3d.s3d_depths, ebx
	add	ebx, MAX_F*2

	mov	d torus3d.s3d_order, ebx
	add	ebx, MAX_F*2

	mov	d torus3d.s3d_point_nrm, ebx
	add	ebx, MAX_P*(size vector3d)

	mov	d torus3d.s3d_r_point_nrm, ebx
	add	ebx, MAX_P*(size vector3d)

	mov	d torus3d.s3d_tex_coords, o tex_coords

	mov	d torus3d.s3d_bmap, ebx
	add	ebx, 16384
	mov	d torus3d.s3d_emap, ebx

	; init maps
	mov	edi, d torus3d.s3d_bmap
	call	calc_bumpmap

	mov	edi, d torus3d.s3d_emap
	call	calc_envmap

	; init normals
	mov	esi, o torus3d
	call	init_point_normals

	; make background
	;mov	edi, ptr_background
	;call	make_tex

	call	init_font

	mov	eax, 0a0000h
	sub	eax, code32_base
	mov	_a0000h, eax

	mov	edi, o _regs
	mov	[edi._eax], 000013h
	mov	eax, 0300h
	mov	ebx, 10h
	int	31h

	mov	esi, o palette
	call	set_palette

@@main_loop:

	mov	esi, o angle_x
	mov	edi, o torus3d
	call	rotate_translate_struct3d

	mov	esi, o torus3d
	call	sort_faces

	mov	esi, o torus3d
	call	render_struct3d
	

comment #
	mov	esi, d torus3d.s3d_t_points
	mov	ecx, d torus3d.s3d_n_points
@@draw:
	lodsw
	movsx	ebx, ax
	lodsw
	movsx	eax, ax

	or	ebx, ebx
	jl	@@next_p
	cmp	ebx, 319
	jg	@@next_p

	or	eax, eax
	jl	@@next_p
	cmp	eax, 199
	jg	@@next_p

	mov	edx, eax
	shl	eax, 6
	shl	edx, 8
	add	eax, edx
	add	eax, ebx
	add	eax, frame_buffer
	mov	b [eax], 31
@@next_p:
	loop	@@draw #

;------------------------------------------------------------

	inc	txt_c1
	cmp	txt_c1, TXT_DELAY
	jb	@@text_draw
	mov	txt_c1, 0

	inc	txt_pos
	mov	eax, txt_curr_len
	cmp	txt_pos, eax
	jbe	@@text_ok

	inc	txt_c2
	cmp	txt_c2, TXT_WAIT
	jb	@@text_draw

	mov	txt_pos, 0
	mov	txt_c2, 0

	; change string
	inc	txt_curr_str
	cmp	txt_curr_str, TXT_N_TEXTS
	jb	@@text_load
	mov	txt_curr_str, 0

@@text_load:
	mov	esi, txt_curr_str
	shl	esi, 2
	add	esi, o txt_offsets
	lodsd
	mov	esi, eax
	movzx	eax, b [esi]
	mov	txt_curr_len, eax
	inc	esi
	mov	txt_curr_ofs, esi
@@text_ok:
	
	mov	esi, txt_curr_ofs
	mov	edi, o txt_buffer
	mov	ecx, txt_pos
	cld
	rep	movsb

	mov	ax, TXT_END_CHAR
	stosw
@@text_draw:

	mov	esi, o txt_buffer
	xor	ecx, ecx
	xor	edx, edx
	mov	eax, TXT_COL
	call	put_string
@@text_end:

;------------------------------------------------------------

	call	retrace
	call	copy_buffer
	call	clear_buffer

	;mov	esi, ptr_background
	;mov	edi, frame_buffer
	;mov	ecx, 64000/4
	;cld
	;rep	movsd
	
	; update angles
	fld	angle_x
	fadd	d_ax
	fstp	angle_x

	fld	angle_y
	fadd	d_ay
	fstp	angle_y

	fld	angle_z
	fadd	d_az
	fstp	angle_z

	in	al, 60h
	dec	al
	jnz	@@main_loop

	mov	edi, o _regs
	mov	[edi._eax], 000003h
	mov	eax, 0300h
	mov	ebx, 10h
	int	31h

	mov	eax, mem_handle
	call	free_mem

@@quit_progy:
	ret
endp

;------------------------------------------------------------
;	in:	edi - offset to 128x128 bumpmap
;	out:	none
;------------------------------------------------------------

calc_bumpmap proc
	push	edi
	mov	ecx, 16384
@@rnd:
	push	ecx
	xor	ecx, ecx
	mov	edx, 255
	call	random
	stosb
	pop	ecx
	loop	@@rnd

	pop	edi
	mov	ecx, 5
@@blur:
	xor	esi, esi
	mov	edx, 16384
	xor	eax, eax
	xor	ebx, ebx
@@b:
	mov	ebp, esi
	dec	ebp
	and	ebp, 16383
	mov	al, b [ebp + edi]

	mov	ebp, esi
	inc	ebp
	and	ebp, 16383
	mov	bl, b [ebp + edi]
	add	eax, ebx

	mov	ebp, esi
	sub	ebp, 128
	and	ebp, 16383
	mov	bl, b [ebp + edi]
	add	eax, ebx

	mov	ebp, esi
	add	ebp, 128
	and	ebp, 16383
	mov	bl, b [ebp + edi]
	add	eax, ebx

	shr	eax, 2
	mov	b [esi + edi], al

	inc	esi
	dec	edx
	jnz	@@b

	loop	@@blur
	ret
endp

;------------------------------------------------------------
;	in:	edi - offset to 128x128 envmap
;	out:	none
;------------------------------------------------------------

calc_envmap proc
	mov	edx, -64
@@ver:
	mov	ecx, -64
@@hor:
	mov	_temp32, ecx
	fild	_temp32
	fdiv	env_div
	fmul	st, st

	mov	_temp32, edx
	fild	_temp32
	fdiv	env_div
	fmul	st, st

	faddp	st(1), st
	fsqrt
	fld1
	fsubrp	st(1), st
	fmul	n_colors
	fmul	env_mul
	fistp	_temp32
	mov	eax, _temp32

	or	eax, eax
	jge	@@ok1
	xor	eax, eax
	jmp	@@ok2
@@ok1:
	cmp	eax, 63
	jle	@@ok2
	mov	eax, 63
@@ok2:
	stosb

	inc	ecx
	cmp	ecx, 64
	jl	@@hor

	inc	edx
	cmp	edx, 64
	jl	@@ver
	ret
endp

comment #
;------------------------------------------------------------
;	in:	edi - offset to 320x200 texture
;	out:	none
;------------------------------------------------------------

make_tex proc
	xor	edx, edx
@@v:
	xor	ecx, ecx
@@h:
	mov	eax, ecx
	xor	eax, edx
	and	eax, 31
	
	mov	_temp32, ecx
	sub	_temp32, 160
	fild	_temp32
	fmul	st, st

	mov	_temp32, edx
	sub	_temp32, 100
	fild	_temp32
	fmul	st, st

	faddp	st(1)
	fsqrt
	fistp	_temp32

	mov	ebx, 255
	sub	ebx, _temp32
	add	eax, ebx
	shr	eax, 3
	stosb

	inc	ecx
	cmp	ecx, 320
	jne	@@h

	inc	edx
	cmp	edx, 200
	jne	@@v
	ret
endp #

include kernel\kernel.inc

include random.asm
include font.asm
include alloc.asm
include graph.asm

include 3dmath.inc
include 3dmath.asm

include engine.inc
include engine.asm

include bump_tri.asm
include b_line.asm

include palette.inc
include torus.inc

OBJECT_MEM equ MAX_P*(size point3d) + MAX_P*(size point2d) + \
	MAX_P*(size vector3d)*2 + MAX_F*2*2 + 2*16384

REQ_MEM	equ 64000 + OBJECT_MEM ;+ 64000

; --- text data ---

TXT_WAIT	equ 20
TXT_COL		equ 63 - 16
TXT_DELAY	equ 7
TXT_END_CHAR	equ 254
TXT_N_TEXTS	equ 6

txt_buffer	db (320/8 * 2) dup(0)
txt_pos		dd 0
txt_c1		dd 0
txt_c2		dd 0

txt_curr_str	dd 0
txt_curr_len	dd S1_LEN
txt_curr_ofs	dd str1 + 1
txt_offsets	dd str1, str2, str3, str4, str5, str6

str1		db S1_LEN, '3D Bump Mapping beta 2.0'
S1_LEN		equ $ - (str1 + 1)

str2		db S2_LEN, 'Coded by Majuma in 32-bit ASM!'
S2_LEN		equ $ - (str2 + 1)

str3		db S3_LEN, 'Contact author: mfelix@polbox.com'
S3_LEN		equ $ - (str3 + 1)

str4		db S4_LEN, 'Visit: www.naag.prv.pl',10
		db 7 dup(32), 'www.ps.nq.pl'
S4_LEN		equ $ - (str4 + 1)

str5		db S5_LEN, 'Greetz: HaRv3sTeR, Klemik, tOudi,',10
		db 'SEM, Stryket, pkmiecik and Zedd'
S5_LEN		equ $ - (str5 + 1)

str6		db S6_LEN, 'Looks cool, huh...?'
S6_LEN		equ $ - (str6 + 1)

; --- end ---

angle_x		dd 0
angle_y		dd 0
angle_z		dd 0

d_ax		dd 0.00859026
d_ay		dd 0.01718059
d_az		dd 0.00429513

n_colors	dd 63.0
env_div		dd 64.0
env_mul		dd 1.2

mem_handle	dd ?
_regs		dpmi_regs ?
_temp32		dd ?
ptr_background	dd ?

torus3d		struct3d ?

code32 ends
end