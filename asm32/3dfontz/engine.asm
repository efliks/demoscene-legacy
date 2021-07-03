.386p
locals

code32 segment para public use32
	assume cs:code32,  ds:code32

comment #
; in: esi = ptr to angles, edi = ptr to struct3d
RotateTranslateStruct3d:
	push	edi
	mov	edi, o rot_matrix
	call	mx_rotation_matrix
	pop	ebp

	mov	esi, d [ebp.s3d_points]
	mov	edi, d [ebp.s3d_r_points]
	mov	ecx, d [ebp.s3d_n_points]
	mov	ebx, o rot_matrix
	call	mx_rotate_points

	mov	esi, d [ebp.s3d_point_nrm]
	mov	edi, d [ebp.s3d_r_point_nrm]
	mov	ecx, d [ebp.s3d_n_points]
	call	mx_rotate_points

	mov	esi, d [ebp.s3d_r_points]
	mov	edi, d [ebp.s3d_t_points]
	mov	ecx, d [ebp.s3d_n_points]
	call	translate_points
	ret
#

;------------------------------------------------------------
;	in:	esi - offset to struct3d
;	out:	none
;------------------------------------------------------------

sort_faces proc
	mov	edi, d [esi.s3d_faces]
	xor	ecx, ecx
	mov	d [esi.s3d_vis_faces], 0
@@add_face:

	movzx	ebx, w [edi.face_v1]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	mov	ax, w [ebx.x2d]
	mov	f_x1, ax
	mov	ax, w [ebx.y2d]
	mov	f_y1, ax

	movzx	ebx, w [edi.face_v2]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	mov	ax, w [ebx.x2d]
	mov	f_x2, ax
	mov	ax, w [ebx.y2d]
	mov	f_y2, ax

	movzx	ebx, w [edi.face_v3]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	mov	ax, w [ebx.x2d]
	mov	f_x3, ax
	mov	ax, w [ebx.y2d]
	mov	f_y3, ax

	mov	ax, f_y1
	sub	ax, f_y3
	mov	bx, f_x2
	sub	bx, f_x1
	imul	bx
	shl	edx, 16
	mov	dx, ax
	push	edx
	mov	ax, f_x1
	sub	ax, f_x3
	mov	bx, f_y2
	sub	bx, f_y1
	imul	bx
	shl	edx, 16
	mov	dx, ax
	pop	ebx
	sub	ebx, edx	
	jl	@@next_face

	movzx	eax, w [edi.face_v1]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, d [esi.s3d_r_points]
	fld	d [eax.z3d]

	movzx	eax, w [edi.face_v2]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, d [esi.s3d_r_points]
	fadd	d [eax.z3d]

	movzx	eax, w [edi.face_v3]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, d [esi.s3d_r_points]
	fadd	d [eax.z3d]

	mov	eax, d [esi.s3d_vis_faces]
	shl	eax, 1
	mov	ebx, eax

	add	eax, d [esi.s3d_order]
	mov	w [eax], cx

	add	ebx, d [esi.s3d_depths]
	fistp	w [ebx]

	inc	d [esi.s3d_vis_faces]
@@next_face:
	add	edi, type face
	inc	ecx
	cmp	ecx, d [esi.s3d_n_faces]
	jne	@@add_face

	mov	eax, d [esi.s3d_vis_faces]
	dec	eax
	push	eax
	push	large 0
	call	quick_sort

	ret
endp

;------------------------------------------------------------
;	in:	esi = offset to struct3d
;		stack: left, right
;	out:	none
;------------------------------------------------------------

quick_sort proc

@@left		equ d [ebp + 08]
@@right		equ d [ebp + 12]

	push	ebp
	mov	ebp, esp

	mov	eax, @@left
	mov	ebx, @@right

	; element=face_depth[(left+right)>>1];
	mov	edi, eax
	add	edi, ebx
	sar	edi, 1
	shl	edi, 1
	add	edi, d [esi.s3d_depths]
	mov	dx, w [edi]

	; while(i<j)
	cmp	eax, ebx
	jge	@@break_main
@@main:

	; while(face_depth[i]>element) i++;
	mov	edi, eax
	shl	edi, 1
	add	edi, d [esi.s3d_depths]
@@small1:
	cmp	w [edi], dx
	jle	@@break_small1
	inc	eax
	add	edi, 2
	jmp	@@small1
@@break_small1:

	; while(face_depth[j]<element) j--;
	mov	edi, ebx
	shl	edi, 1
	add	edi, d [esi.s3d_depths]
@@small2:
	cmp	w [edi], dx
	jge	@@break_small2
	dec	ebx
	sub	edi, 2
	jmp	@@small2
@@break_small2:

	; if(i<=j)
	cmp	eax, ebx
	jg	@@skip_xchg

	mov	edi, d [esi.s3d_depths]
	mov	cx, w [edi + eax*2]
	xchg	cx, w [edi + ebx*2]
	mov	w [edi + eax*2], cx
	
	mov	edi, d [esi.s3d_order]
	mov	cx, w [edi + eax*2]
	xchg	cx, w [edi + ebx*2]
	mov	w [edi + eax*2], cx

	inc	eax
	dec	ebx

@@skip_xchg:
	cmp	eax, ebx
	jl	@@main

@@break_main:

	; if(j>left) depth_sorting(left, j);
	cmp	ebx, @@left
	jle	@@skip_call1

	push	eax

	push	ebx
	push	@@left
	call	quick_sort

	pop	eax

@@skip_call1:

	; if(i<right) depth_sorting(i, right);
	cmp	eax, @@right
	jge	@@skip_call2

	push	@@right
	push	eax
	call	quick_sort

@@skip_call2:

	pop	ebp
	ret	8
endp

;------------------------------------------------------------

; in: edi = ptr to struct3d
CenterStruct3d:
	fldz
	fst	q [str3d_ox]
	fst	q [str3d_oy]
	fstp	q [str3d_oz]


	mov	ebp, d [edi.s3d_points]
	mov	ecx, d [edi.s3d_n_points]
CS3d_add:
	fld	q [str3d_ox]
	fadd	d [ebp.x3d]
	fstp	q [str3d_ox]

	fld	q [str3d_oy]
	fadd	d [ebp.y3d]
	fstp	q [str3d_oy]

	fld	q [str3d_oz]
	fadd	d [ebp.z3d]
	fstp	q [str3d_oz]
	add	ebp, size point3d
	loop	CS3d_add


	fld	q [str3d_ox]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_ox]

	fld	q [str3d_oy]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_oy]

	fld	q [str3d_oz]
	fidiv	d [edi.s3d_n_points]
	fstp	q [str3d_oz]


	mov	ebp, d [edi.s3d_points]
	mov	ecx, d [edi.s3d_n_points]
CS3d_move:
	fld	d [ebp.x3d]
	fsub	q [str3d_ox]
	fstp	d [ebp.x3d]

	fld	d [ebp.y3d]
	fsub	q [str3d_oy]
	fstp	d [ebp.y3d]

	fld	d [ebp.z3d]
	fsub	q [str3d_oz]
	fstp	d [ebp.z3d]

	add	ebp, size point3d
	loop	CS3d_move
	ret

;------------------------------------------------------------

; in: edi = ptr to struct3d, filled ScaleX, ScaleY, ScaleZ 
ScaleStruct3d:
	mov	ecx, d [edi.s3d_n_points]
	mov	ebx, d [edi.s3d_points]
SS3d_scale:
	fld	d [ebx.x3d]
	fmul	d [ScaleX]
	fstp	d [ebx.x3d]

	fld	d [ebx.y3d]
	fmul	d [ScaleY]
	fstp	d [ebx.y3d]

	fld	d [ebx.z3d]
	fmul	d [ScaleZ]
	fstp	d [ebx.z3d]

	add	ebx, size point3d
	loop	SS3d_scale
	ret

;------------------------------------------------------------

; in: esi = ptr to struct3d
DrawGouraudFaces:
	call	sort_faces

	mov	edi, d [esi.s3d_order]
	mov	ecx, d [esi.s3d_vis_faces]
DGF_face:
	push	edi
	push	ecx

	movzx	eax, w [edi]
	mov	ebx, eax
	shl	eax, 1
	shl	ebx, 2
	add	ebx, eax
	add	ebx, d [esi.s3d_faces]
	push	ebx

	mov	edi, o GrdX1
	mov	ecx, 3
DGF_load_vert:
	movzx	eax, w [ebx]
	shl	eax, 2
	add	eax, d [esi.s3d_t_points]
	movsx	edx, w [eax.x2d]
	mov	d [edi + 0], edx
	movsx	edx, w [eax.y2d]
	mov	d [edi + 4], edx
	add	edi, 8
	add	ebx, 2
	dec	ecx
	jnz	DGF_load_vert

	pop	ebx
	mov	edi, o GrdCol1
	mov	ecx, 3
DGF_load_col:
	movzx	eax, w [ebx]
	mov	edx, eax
	shl	eax, 2
	shl	edx, 3
	add	eax, edx
	add	eax, d [esi.s3d_r_point_nrm]

	push	esi
	push	edi
	mov	esi, o LightVector
	mov	edi, eax
	call	dot_product
	pop	edi
	pop	esi

	call	MakeColor
	stosd

	add	ebx, 2
	dec	ecx
	jnz	DGF_load_col

	call	GouraudTriangle

	pop	ecx
	pop	edi
	add	edi, 2
	dec	ecx
	jnz	DGF_face
	ret

;------------------------------------------------------------
;	in:	esi - offset to struct3d
;	out:	none
;------------------------------------------------------------

init_point_normals proc
	xor	ecx, ecx
@@make_n:

	mov	sum_x, 0
	mov	sum_y, 0
	mov	sum_z, 0

	mov	n_hit, 0

	mov	ebp, d [esi.s3d_faces]
	mov	edx, d [esi.s3d_n_faces]
@@f:
	
	movzx	eax, w [ebp.face_v1]
	cmp	eax, ecx
	je	@@face_hit
	movzx	eax, w [ebp.face_v2]
	cmp	eax, ecx
	je	@@face_hit
	movzx	eax, w [ebp.face_v3]
	cmp	eax, ecx
	je	@@face_hit

	jmp	@@next_f
@@face_hit:
	inc	n_hit

	; make face-normal
	push	edx
	mov	edx, esi
	
	
	movzx	esi, w [ebp.face_v1]
	mov	eax, esi
	shl	esi, 2
	shl	eax, 3
	add	esi, eax
	add	esi, d [edx.s3d_points]

	movzx	edi, w [ebp.face_v2]
	mov	eax, edi
	shl	edi, 2
	shl	eax, 3
	add	edi, eax
	add	edi, d [edx.s3d_points]

	mov	ebx, o vec1
	call	make_vector

	mov	esi, edi

	movzx	edi, w [ebp.face_v3]
	mov	eax, edi
	shl	edi, 2
	shl	eax, 3
	add	edi, eax
	add	edi, d [edx.s3d_points]

	mov	ebx, o vec2
	call	make_vector

	mov	esi, o vec1
	mov	edi, o vec2
	mov	ebx, o vec3
	call	cross_product

	
	fld	sum_x
	fadd	vec3.vec_x
	fstp	sum_x

	fld	sum_y
	fadd	vec3.vec_y
	fstp	sum_y

	fld	sum_z
	fadd	vec3.vec_z
	fstp	sum_z


	mov	esi, edx
	pop	edx
@@next_f:
	add	ebp, size face
	dec	edx
	jnz	@@f


	mov	eax, ecx
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, d [esi.s3d_point_nrm]

	fld	sum_x
	fidiv	n_hit
	fstp	d [eax.vec_x]

	fld	sum_y
	fidiv	n_hit
	fstp	d [eax.vec_y]

	fld	sum_z
	fidiv	n_hit
	fstp	d [eax.vec_z]

	mov	edi, eax
	call	normalize_vector

	inc	ecx
	cmp	ecx, d [esi.s3d_n_points]
	jne	@@make_n
	ret
endp

;------------------------------------------------------------

; in: st0 = dot-product
MakeColor:
	fcom	d [DotMin]
	fstsw	ax
	sahf
	jb	MC_err
	fcom	d [DotMax]
	fstsw	ax
	sahf
	ja	MC_err
	
	fmul	d [GouraudColors]
	fistp	d [MC_temp]
	mov	eax, d [MC_temp]
	ret
MC_err:
	ffree	st
	xor	eax, eax
	ret

;------------------------------------------------------------

sum_x		dd ?
sum_y		dd ?
sum_z		dd ?
n_hit		dd ?

vec1		vector3d ?
vec2		vector3d ?
vec3		vector3d ?

rot_matrix	matrix ?

f_x1		dw ?
f_y1		dw ?
f_x2		dw ?
f_y2		dw ?
f_x3		dw ?
f_y3		dw ?

str3d_ox	dq ?
str3d_oy	dq ?
str3d_oz	dq ?

ScaleX		dd LETGEN_SCALEX
ScaleY		dd LETGEN_SCALEY
ScaleZ		dd LETGEN_SCALEZ

LightVector	vector3d <0.0,0.0,-1.0>
GouraudColors	dd 63.0

DotMin		dd 0.0
DotMax		dd 1.0
MC_temp		dd ?

code32 ends
