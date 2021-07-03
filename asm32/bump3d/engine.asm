.386p
locals

b	equ byte ptr
w	equ word ptr
d	equ dword ptr
o	equ offset

code32 segment para public use32
	assume cs:code32,  ds:code32

;------------------------------------------------------------
;	in:	esi - offset to angles
;		edi - offset to struct3d
;	out:	none
;------------------------------------------------------------

rotate_translate_struct3d proc
	mov	ebp, edi

	; make rotation matrix
	mov	edi, o rot_mx
	call	mx_rotation_matrix

	; rotate vertices
	mov	ebx, edi
	mov	esi, d [ebp.s3d_points]
	mov	edi, d [ebp.s3d_r_points]
	mov	ecx, d [ebp.s3d_n_points]
	call	mx_rotate_points

	; rotate normals
	mov	esi, d [ebp.s3d_point_nrm]
	mov	edi, d [ebp.s3d_r_point_nrm]
	mov	ecx, d [ebp.s3d_n_points]
	call	mx_rotate_points

	; translate vertices to 2d
	mov	esi, d [ebp.s3d_r_points]
	mov	edi, d [ebp.s3d_t_points]
	mov	ecx, d [ebp.s3d_n_points]
	call	translate_points

	ret
endp

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
;	in:	esi - offset to struct3d
;		stack: left,  right
;	out:	none
;------------------------------------------------------------

quick_sort proc

@@left equ d [ebp+8]
@@right equ d [ebp+12]

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
;	in:	esi - offset to struct3d
;	out:	none
;------------------------------------------------------------

render_struct3d proc
	mov	eax, d [esi.s3d_bmap]
	mov	t_bmap, eax
	mov	eax, d [esi.s3d_emap]
	mov	t_emap, eax

	mov	edi, d [esi.s3d_order]
	mov	ecx, d [esi.s3d_vis_faces]
@@draw:
	push	esi
	push	edi
	push	ecx

	movzx	edi, w [edi]
	mov	edx, edi
	mov	eax, edi
	shl	edi, 1
	shl	eax, 2
	add	edi, eax
	add	edi, d [esi.s3d_faces]

	
	push	edi
	mov	ebp, o t_x1
	mov	ecx, 3
@@load_tri:
	movzx	eax, w [edi]
	shl	eax, 2
	add	eax, d [esi.s3d_t_points]
	movsx	ebx, w [eax.x2d]
	mov	d [ebp], ebx
	movsx	ebx, w [eax.y2d]
	mov	d [ebp + 4], ebx
	add	edi, 2
	add	ebp, 8
	dec	ecx
	jnz	@@load_tri
	pop	edi


comment #
	movzx	ebx, w [edi.face_v1]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	movsx	eax, w [ebx.x2d]
	mov	t_x1, eax
	mov	ax, w [ebx.y2d]
	movsx	eax, ax
	mov	t_y1, eax

	movzx	ebx, w [edi.face_v2]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	mov	ax, w [ebx.x2d]
	movsx	eax, ax
	mov	t_x2, eax
	mov	ax, w [ebx.y2d]
	movsx	eax, ax
	mov	t_y2, eax

	movzx	ebx, w [edi.face_v3]
	shl	ebx, 2
	add	ebx, d [esi.s3d_t_points]
	mov	ax, w [ebx.x2d]
	movsx	eax, ax
	mov	t_x3, eax
	mov	ax, w [ebx.y2d]
	movsx	eax, ax
	mov	t_y3, eax #


	mov	eax, edx
	shl	edx, 1
	shl	eax, 2
	add	edx, eax
	
	add	edx, d [esi.s3d_tex_coords]
	push	edi
	mov	edi, o b_x1
	mov	ecx, 6
@@load_tex:
	movzx	eax, b [edx]
	stosd
	inc	edx
	dec	ecx
	jnz	@@load_tex


	pop	edx
	mov	edi, o e_x1
	mov	ecx, 3
@@load_env:
	movzx	eax, w [edx]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	ebx, eax
	add	ebx, d [esi.s3d_r_point_nrm]

	fld	d [ebx.vec_x]
	fmul	_63
	fadd	_63
	fistp	d [edi]
	
	fld	d [ebx.vec_y]
	fmul	_63
	fadd	_63
	fistp	d [edi + 4]

	add	edx, 2
	add	edi, 8
	dec	ecx
	jnz	@@load_env

	call	bump_triangle

@@next_face:
	pop	ecx
	pop	edi
	pop	esi
	add	edi, 2
	dec	ecx
	jnz	@@draw

	ret
endp

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

_63	dd 63.0

sum_x	dd ?
sum_y	dd ?
sum_z	dd ?
n_hit	dd ?

vec1	vector3d ?
vec2	vector3d ?
vec3	vector3d ?

rot_mx	matrix ?

f_x1	dw ?
f_y1	dw ?
f_x2	dw ?
f_y2	dw ?
f_x3	dw ?
f_y3	dw ?

code32 ends