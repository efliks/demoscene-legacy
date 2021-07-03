;------------------------------------------------------------
;	AsmShade
;	Coded by Mikolaj Felix
;	mfelix@polbox.com
;
;	Updated: 13/10/01
;------------------------------------------------------------

.model tiny
.386
locals
jumps



.code
org 100h

start:
	finit

	call	alloc_seg
	or	ax, ax
	jz	quit_progy
	mov	buffer_seg, ax
	call	alloc_seg
	or	ax, ax
	jz	quit_progy
	mov	envmap_seg, ax

	call	init_sincos
	call	init_font

	call	init_face_normals
	call	init_point_normals

	call	init_envmap


	mov	ax, 13h
	int	10h

	mov	si, offset palette
	call	set_palette

	call	get_sec
	mov	t_before, al
	mov	last_frames, 0
	mov	byte ptr txt_framerate, 0

main_loop:
	mov	si, offset angle_x
	mov	di, offset rot_matrix
	call	mx_rotation_matrix

	mov	si, offset points
	mov	di, offset rotated_points
	mov	bx, offset rot_matrix
	mov	cx, MAX_P
	call	mx_rotate_points

	
	cmp	shade_model, FLAT_SHADING
	je	rot_flat

	; rotate point-normals
	mov	si, offset point_normals
	mov	di, offset rotated_point_normals
	mov	bx, offset rot_matrix
	mov	cx, MAX_P
	call	mx_rotate_points
	jmp	translate

rot_flat:
	; rotate face-normals
	mov	si, offset face_normals
	mov	di, offset rotated_face_normals
	mov	bx, offset rot_matrix
	mov	cx, MAX_F
	call	mx_rotate_points

translate:
	mov	si, offset rotated_points
	mov	di, offset translated_points
	mov	cx, MAX_P
	call	translate_points

	call	sort_faces
	call	draw_faces

comment #
	mov	si, o translated_points
	mov	cx, MAX_P
	mov	ax, w [buffer_seg]
	push	es
	mov	es, ax
_draw_points:
	mov	ax, w [si.y2d]
	mov	bx, 320
	mul	bx
	add	ax, w [si.x2d]
	mov	di, ax
	mov	al, 60
	stosb
	add	si, size point2d
	loop	_draw_points
	pop	es #


	; update angles
	add	word ptr angle_x, 1
	add	word ptr angle_y, 3
	add	word ptr angle_z, 1

	
	; write model info
	mov	si, txt_model_ofs
	xor	cx, cx
	xor	dx, dx
	mov	al, TEXT_COLOR
	call	put_string

	; write framerate
	call	get_sec
	cmp	al, t_before
	je	wf_skip

	mov	t_before, al

	mov	ax, last_frames
	mov	di, offset txt_framerate
	call	itoa

	mov	last_frames, 0
wf_skip:
	mov	si, offset txt_fps
	xor	cx, cx
	mov	dx, 12
	mov	al, TEXT_COLOR
	call	put_string

	mov	si, offset txt_framerate
	mov	cx, 152
	mov	dx, 12
	mov	al, TEXT_COLOR
	call	put_string

	; write if full speed mode is enabled
	cmp	delay, 1
	je	no_full
	mov	si, offset txt_no_delay
	xor	cx, cx
	mov	dx, 24
	mov	al, TEXT_COLOR
	call	put_string
	jmp	flip_buffers
no_full:
	call	retrace
flip_buffers:
	call	copy_buffer
	call	clear_buffer

	inc	last_frames


	; read keyboard
	mov	ah, 01h
	int	16h
	jnz	key_hit
	jmp	main_loop
key_hit:
	xor	ah, ah
	int	16h
	cmp	al, 27				; escape
	je	break_main_loop
	cmp	al, 32				; space
	je	change_shade_model
	cmp	al, 13
	je	change_speed_mode
	jmp	main_loop

change_speed_mode:
	inc	delay
	cmp	delay, 2
	jb	main_loop
	mov	delay, 0
	jmp	main_loop

change_shade_model:
	inc	shade_model
	cmp	shade_model, ENV_MAPPING+1
	jb	sm_ok
	mov	shade_model, FLAT_SHADING
sm_ok:
	cmp	shade_model, FLAT_SHADING
	je	load_text1
	cmp	shade_model, GOURAUD_SHADING
	je	load_text2

	mov	ax, offset txt_model_env
	mov	txt_model_ofs, ax
	jmp	main_loop
load_text1:
	mov	ax, offset txt_model_flat
	mov	txt_model_ofs, ax
	jmp	main_loop
load_text2:
	mov	ax, offset txt_model_gouraud
	mov	txt_model_ofs, ax
	jmp	main_loop


break_main_loop:
	mov	ax, 03h
	int	10h
	
quit_progy:
	mov	ax, buffer_seg
	call	free_seg
	mov	ax, envmap_seg
	call	free_seg

	mov	dx, offset txt_outro
	mov	ah, 09h
	int	21h
	mov	ah, 4ch
	int	21h

sort_faces proc

@@x1 equ word ptr [bp-2]
@@y1 equ word ptr [bp-4]
@@x2 equ word ptr [bp-6]
@@y2 equ word ptr [bp-8]
@@x3 equ word ptr [bp-10]
@@y3 equ word ptr [bp-12]

	push	bp
	mov	bp, sp
	sub	sp, 12

	mov	visible_faces, 0
	mov	si, offset faces
	xor	cx, cx
sf_add_face:

	mov	bx, word ptr [si.face_v1]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x1, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y1, ax

	mov	bx, word ptr [si.face_v2]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x2, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y2, ax

	mov	bx, word ptr [si.face_v3]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x3, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y3, ax

	mov	ax, @@y1
	sub	ax, @@y3
	mov	bx, @@x2
	sub	bx, @@x1
	imul	bx
	shl	edx, 16
	mov	dx, ax
	push	edx
	mov	ax, @@x1
	sub	ax, @@x3
	mov	bx, @@y2
	sub	bx, @@y1
	imul	bx
	shl	edx, 16
	mov	dx, ax
	pop	ebx
	sub	ebx, edx			; normal_z = (y1-y3)*(x2-x1)-(x1-x3)*(y2-y1)
	jl	sf_next_face			; normal_z < 0 (face invisible,  skip it)

	mov	ax, word ptr [si.face_v1]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	bx, ax
	add	bx, offset rotated_points
	fld	dword ptr [bx.z3d]

	mov	ax, word ptr [si.face_v2]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	bx, ax
	add	bx, offset rotated_points
	fadd	dword ptr [bx.z3d]

	mov	ax, word ptr [si.face_v3]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	bx, ax
	add	bx, offset rotated_points
	fadd	dword ptr [bx.z3d]

	mov	bx, visible_faces
	shl	bx, 1
	fistp	word ptr face_depths[bx]

	mov	word ptr face_order[bx], cx
	inc	visible_faces

sf_next_face:
	add	si, type face
	inc	cx
	cmp	cx, MAX_F
	jne	sf_add_face

	mov	ax, visible_faces
	dec	ax
	push	ax
	push	word ptr 0
	call	depth_sorting
	
	mov	sp, bp
	pop	bp
	ret
endp

;------------------------------------------------------------
;	in:	left,  right
;	out:	none
;------------------------------------------------------------

; quicksort-type sorting routine
depth_sorting proc

@@left equ word ptr [bp+4]
@@right equ word ptr [bp+6]

	push	bp
	mov	bp, sp

	mov	ax, @@left
	mov	bx, @@right

	; element=face_depth[(left+right)>>1];
	mov	si, ax
	add	si, bx
	sar	si, 1
	shl	si, 1
	mov	dx, word ptr face_depths[si]

	; while(i<j)
	cmp	ax, bx
	jge	dso_break_main
dso_main:

	; while(face_depth[i]<element) i++;
	mov	si, ax
	shl	si, 1
	add	si, offset face_depths
dso_small1:
	cmp	word ptr [si], dx
	jge	dso_break_small1
	inc	ax
	add	si, 2
	jmp	dso_small1
dso_break_small1:

	; while(face_depth[j]>element) j--;
	mov	si, bx
	shl	si, 1
	add	si, offset face_depths
dso_small2:
	cmp	word ptr [si], dx
	jle	dso_break_small2
	dec	bx
	sub	si, 2
	jmp	dso_small2
dso_break_small2:

	; if(i<=j)
	cmp	ax, bx
	jg	dso_skip_xchg

	mov	si, ax
	shl	si, 1
	mov	di, bx
	shl	di, 1

	mov	cx, word ptr face_depths[si]
	xchg	cx, word ptr face_depths[di]
	mov	word ptr face_depths[si], cx

	mov	cx, word ptr face_order[si]
	xchg	cx, word ptr face_order[di]
	mov	word ptr face_order[si], cx

	inc	ax
	dec	bx

dso_skip_xchg:
	cmp	ax, bx
	jl	dso_main

dso_break_main:

	; if(j>left) depth_sorting(left, j);
	cmp	bx, @@left
	jle	dso_skip_call1

	push	ax

	push	bx
	push	@@left
	call	depth_sorting

	pop	ax

dso_skip_call1:

	; if(i<right) depth_sorting(i, right);
	cmp	ax, @@right
	jge	dso_skip_call2

	push	@@right
	push	ax
	call	depth_sorting

dso_skip_call2:

	pop	bp
	ret	4
endp

;------------------------------------------------------------
;	in:	st(0) - dot-product
;	out:	al - color
;------------------------------------------------------------

make_color proc
	push	bp
	mov	bp, sp
	sub	sp, 2

	fcom	dot_min
	fstsw	ax
	sahf
	ja	mc_ok1
	ffree	st
	xor	ax, ax
	jmp	mc_done
mc_ok1:
	fcom	dot_max
	fstsw	ax
	sahf
	jb	mc_ok2
	ffree	st
	xor	ax, ax
	jmp	mc_done
mc_ok2:

	fmul	max_phong_colors
	fistp	word ptr [bp-2]
	mov	ax, word ptr [bp-2]
mc_done:

	mov	sp, bp
	pop	bp
	ret
endp

draw_faces proc

; face vertices
@@x1 equ word ptr [bp-2]
@@y1 equ word ptr [bp-4]
@@x2 equ word ptr [bp-6]
@@y2 equ word ptr [bp-8]
@@x3 equ word ptr [bp-10]
@@y3 equ word ptr [bp-12]

; gouraud colors
@@c1 equ word ptr [bp-14]
@@c2 equ word ptr [bp-16]
@@c3 equ word ptr [bp-18]

; texture vertices
@@tx1 equ word ptr [bp-20]
@@ty1 equ word ptr [bp-22]
@@tx2 equ word ptr [bp-24]
@@ty2 equ word ptr [bp-26]
@@tx3 equ word ptr [bp-28]
@@ty3 equ word ptr [bp-30]

	push	bp
	mov	bp, sp
	sub	sp, 30

	mov	si, offset face_order
	mov	bx, visible_faces
	mov	cx, bx
	shl	bx, 1
	sub	bx, 2
	add	si, bx
df_draw:
	push	cx
	push	si

	mov	ax, word ptr [si]
	mov	dx, ax				; dx - face number (to find it's normal later)
	mov	si, ax
	shl	si, 1
	shl	ax, 2
	add	si, ax
	add	si, offset faces			; si - offset to current face

	; read points of face
	mov	bx, word ptr [si.face_v1]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x1, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y1, ax

	mov	bx, word ptr [si.face_v2]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x2, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y2, ax

	mov	bx, word ptr [si.face_v3]
	shl	bx, 2
	add	bx, offset translated_points
	mov	ax, word ptr [bx.x2d]
	mov	@@x3, ax
	mov	ax, word ptr [bx.y2d]
	mov	@@y3, ax


	; select shade model
	cmp	shade_model, FLAT_SHADING
	je	df_flat
	cmp	shade_model, GOURAUD_SHADING	
	je	df_gouraud

	; do environment mapping
	mov	cx, 3
	mov	di, bp
	sub	di, 20
df_env_coord:
	lodsw
	mov	bx, ax
	shl	bx, 2
	shl	ax, 3
	add	bx, ax
	add	bx, offset rotated_point_normals

	; texture x = (rotated_point_normal->x * 63) + 63
	fld	dword ptr [bx.vec_x]
	fmul	correct_tex
	fadd	correct_tex
	fistp	word ptr [di]			; ds = ss

	; texture y = (rotated_point_normal->y * 63) + 63
	fld	dword ptr [bx.vec_y]
	fmul	correct_tex
	fadd	correct_tex
	fistp	word ptr [di-2]

	sub	di, 4
	dec	cx
	jnz	df_env_coord

	push	@@ty3
	push	@@tx3
	push	@@ty2
	push	@@tx2
	push	@@ty1
	push	@@tx1
	push	envmap_seg			; texture segment
	push	word ptr 0			; texture offset
	push	@@y3
	push	@@x3
	push	@@y2
	push	@@x2
	push	@@y1
	push	@@x1
	call	textured_triangle

	jmp	df_next_face

df_flat:
	mov	di, dx
	shl	di, 2
	shl	dx, 3
	add	di, dx
	add	di, offset rotated_face_normals
	
comment #
	; rotate normal we're interested in
	mov	si, di
	add	si, offset face_normals
	add	di, offset rotated_face_normals
	push	di
	mov	bx, offset rot_matrix
	mov	cx, 1
	call	mx_rotate_points
	pop	di #

	mov	si, offset light_vector
	call	dot_product
	call	make_color

	push	ax
	push	@@y3
	push	@@x3
	push	@@y2
	push	@@x2
	push	@@y1
	push	@@x1
	call	flat_triangle

	jmp	df_next_face

df_gouraud:
	mov	cx, 3
	mov	di, bp
	sub	di, 14
df_load_col:
	lodsw					; si - currect face,  read vertex nr
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	bx, ax
	add	bx, offset rotated_point_normals

	push	si
	push	di
	mov	si, offset light_vector
	mov	di, bx
	call	dot_product
	pop	di
	pop	si

	call	make_color
	mov	word ptr [di], ax

	sub	di, 2
	dec	cx
	jnz	df_load_col

	push	@@c3
	push	@@c2
	push	@@c1
	push	@@y3
	push	@@x3
	push	@@y2
	push	@@x2
	push	@@y1
	push	@@x1
	call	gouraud_triangle

df_next_face:
	pop	si
	pop	cx
	sub	si, 2
	dec	cx
	jnz	df_draw

	mov	sp, bp
	pop	bp
	ret
endp

init_face_normals proc

@@p1_ofs equ word ptr [bp-2]
@@p2_ofs equ word ptr [bp-4]
@@p3_ofs equ word ptr [bp-6]

@@v1 equ [bp-18]
@@v2 equ [bp-30]

	push	bp
	mov	bp, sp
	sub	sp, 30

	mov	si, offset faces
	mov	di, offset face_normals
	mov	cx, MAX_F
ifn_loop:
	mov	ax, word ptr [si.face_v1]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	ax, bx
	add	ax, offset points
	mov	@@p1_ofs, ax

	mov	ax, word ptr [si.face_v2]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	ax, bx
	add	ax, offset points
	mov	@@p2_ofs, ax

	mov	ax, word ptr [si.face_v3]
	mov	bx, ax
	shl	ax, 2
	shl	bx, 3
	add	ax, bx
	add	ax, offset points
	mov	@@p3_ofs, ax

	push	si
	push	di
	mov	si, @@p1_ofs
	mov	di, @@p2_ofs
	mov	bx, bp
	sub	bx, 18
	call	make_vector
	mov	si, @@p2_ofs
	mov	di, @@p3_ofs
	mov	bx, bp
	sub	bx, 30
	call	make_vector
	pop	di
	pop	si

	push	si
	push	di
	mov	bx, di
	mov	si, bp				; ds = ss
	sub	si, 18
	mov	di, bp
	sub	di, 30
	call	cross_product
	pop	di
	pop	si

	call	normalize_vector

	add	si, type face
	add	di, type vector3d
	dec	cx
	jnz	ifn_loop

	mov	sp, bp
	pop	bp
	ret
endp

init_point_normals proc

@@x equ dword ptr [bp-4]
@@y equ dword ptr [bp-8]
@@z equ dword ptr [bp-12]
@@point_number equ word ptr [bp-26]
@@hit_faces equ word ptr [bp-28]

	push	bp
	mov	bp, sp
	sub	sp, 28

	mov	di, offset point_normals
	mov	@@point_number, 0
ipn_loop:

	mov	@@hit_faces, 0
	mov	@@x, 0
	mov	@@y, 0
	mov	@@z, 0
	mov	si, offset faces
	xor	cx, cx
ipn_check_face:
	xor	bx, bx
ipn_check_vertex:
	mov	ax, word ptr [si][bx]
	cmp	ax, @@point_number
	jne	ipn_next_vertex

	push	si
	mov	si, cx
	mov	ax, si
	shl	si, 2
	shl	ax, 3
	add	si, ax
	add	si, offset face_normals

	fld	@@x
	fadd	dword ptr [si.vec_x]
	fstp	@@x
	fld	@@y
	fadd	dword ptr [si.vec_y]
	fstp	@@y
	fld	@@z
	fadd	dword ptr [si.vec_z]
	fstp	@@z
	pop	si

	inc	@@hit_faces
	jmp	ipn_next_face

ipn_next_vertex:
	add	bx, 2
	cmp	bx, 6
	jne	ipn_check_vertex

ipn_next_face:
	add	si, type face
	inc	cx
	cmp	cx, MAX_F
	jne	ipn_check_face

	fld	@@x
	fidiv	@@hit_faces
	fstp	dword ptr [di.vec_x]
	fld	@@y
	fidiv	@@hit_faces
	fstp	dword ptr [di.vec_y]
	fld	@@z
	fidiv	@@hit_faces
	fstp	dword ptr [di.vec_z]

	call	normalize_vector

	add	di, type vector3d
	inc	@@point_number
	cmp	@@point_number, MAX_P
	jne	ipn_loop

	mov	sp, bp
	pop	bp
	ret
endp

init_envmap proc

@@temp equ word ptr [bp-2]

	push	bp
	mov	bp, sp
	sub	sp, 2

	push	es
	mov	es, envmap_seg
	xor	di, di

	mov	dx, -64
ie_ver:
	mov	cx, -64
ie_hor:
	mov	@@temp, cx
	fild	@@temp
	fmul	st, st
	mov	@@temp, dx
	fild	@@temp
	fmul	st, st
	faddp	st(1), st
	fsqrt
	mov	@@temp, 62
	fisubr	@@temp
	fmul	env_const
	fistp	@@temp
	mov	ax, @@temp

	or	ax, ax
	jge	ie_ok1
	xor	ax, ax
	jmp	ie_ok2
ie_ok1:
	cmp	ax, 62
	jle	ie_ok2
	mov	ax, 62
ie_ok2:
	stosb

	inc	cx
	cmp	cx, 64
	jne	ie_hor

	inc	dx
	cmp	dx, 64
	jne	ie_ver

	pop	es

	mov	sp, bp
	pop	bp
	ret
endp

get_sec proc
	xor	ax, ax
	out	70h, al
	in	al, 71h
	ret
endp

include asmshade.inc
include mem.asm	
include graph.asm
include font.asm
include flat3.asm
include grd3.asm
include tex3.asm
include 3dmath.asm
include itoa.asm

include torus.inc
include palette.inc

.data
	txt_fps	db 'FPS (in last sec): ', 0

	shade_model db FLAT_SHADING
	
	txt_model_flat db 'Flat Shading', 0
	txt_model_gouraud db 'Gouraud Shading', 0
	txt_model_env db 'Environment Mapping', 0
	txt_model_ofs dw offset txt_model_flat

	txt_outro db 'AsmShade by Mikolaj Felix 13/10/01', 10, 13
		db 'mfelix@polbox.com', 10, 13, 10, 13, '$'

	angle_x dw 0
	angle_y dw 0
	angle_z dw 0

	dot_min	dd 0.0
	dot_max	dd 1.0
	max_phong_colors dd 62.0

	light_vector vector3d <0.0, 0.0, -1.0>

	env_const dd 1.2
	correct_tex dd 63.0

	delay db 1
	txt_no_delay db 'Full speed mode', 0
.data?
	envmap_seg dw ?

	txt_framerate db 16 dup(?)
	t_before db ?
	last_frames dw ?

	rot_matrix matrix ?

	rotated_points point3d MAX_P dup(?)
	translated_points point2d MAX_P dup(?)

	face_normals vector3d MAX_F dup(?)
	rotated_face_normals vector3d MAX_F dup(?)

	point_normals vector3d MAX_P dup(?)
	rotated_point_normals vector3d MAX_P dup(?)

	visible_faces dw ?
	face_depths dw MAX_F dup(?)
	face_order dw MAX_F dup(?)

end start
