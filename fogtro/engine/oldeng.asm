
;------------------------------------------------------------
;	in:	esi - offset to struct3d
;	out:	none
;------------------------------------------------------------

sort_faces proc
	mov	edi, dword ptr [esi.s3d_faces]
	xor	ecx, ecx
	mov	dword ptr [esi.s3d_vis_faces], 0
@@add_face:
	movzx	ebx, word ptr [edi.face_v1]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	mov	f_x1, ax
	mov	ax, word ptr [ebx.y2d]
	mov	f_y1, ax

	movzx	ebx, word ptr [edi.face_v2]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	mov	f_x2, ax
	mov	ax, word ptr [ebx.y2d]
	mov	f_y2, ax

	movzx	ebx, word ptr [edi.face_v3]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	mov	f_x3, ax
	mov	ax, word ptr [ebx.y2d]
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

	movzx	eax, word ptr [edi.face_v1]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, dword ptr [esi.s3d_r_points]
	fld	dword ptr [eax.z3d]

	movzx	eax, word ptr [edi.face_v2]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, dword ptr [esi.s3d_r_points]
	fadd	dword ptr [eax.z3d]

	movzx	eax, word ptr [edi.face_v3]
	mov	ebx, eax
	shl	eax, 2
	shl	ebx, 3
	add	eax, ebx
	add	eax, dword ptr [esi.s3d_r_points]
	fadd	dword ptr [eax.z3d]


	fcom	min_d
	fstsw	ax
	sahf
	ja	@@dist_ok

	ffree	st
	jmp	@@next_face

comment #
	fcom	d [min_d]
	fstsw	ax
	sahf
	ja	_sf_check1
	ffree	st(0)
	jmp	@@next_face

_sf_check1:
	fcom	d [max_d]
	fstsw	ax
	sahf
	jb	@@dist_ok
	ffree	st(0)
	jmp	@@next_face #

@@dist_ok:
	mov	eax, dword ptr [esi.s3d_vis_faces]
	shl	eax, 1
	mov	ebx, eax

	add	eax, dword ptr [esi.s3d_order]
	mov	word ptr [eax], cx

	add	ebx, dword ptr [esi.s3d_depths]
	fistp	word ptr [ebx]

	inc	dword ptr [esi.s3d_vis_faces]
@@next_face:
	add	edi, type face
	inc	ecx
	cmp	ecx, dword ptr [esi.s3d_n_faces_]
	jne	@@add_face

	mov	eax, dword ptr [esi.s3d_vis_faces]
	or	eax, eax
	jnz	@@launch_qsort
	ret
@@launch_qsort:
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

@@left equ dword ptr [ebp+8]
@@right equ dword ptr [ebp+12]

	push	ebp
	mov	ebp, esp

	mov	eax, @@left
	mov	ebx, @@right

	; element=face_depth[(left+right)>>1];
	mov	edi, eax
	add	edi, ebx
	sar	edi, 1
	shl	edi, 1
	add	edi, dword ptr [esi.s3d_depths]
	mov	dx, word ptr [edi]

	; while(i<j)
	cmp	eax, ebx
	jge	@@break_main
@@main:

	; while(face_depth[i]>element) i++;
	mov	edi, eax
	shl	edi, 1
	add	edi, dword ptr [esi.s3d_depths]
@@small1:
	cmp	word ptr [edi], dx
	jle	@@break_small1
	inc	eax
	add	edi, 2
	jmp	@@small1
@@break_small1:

	; while(face_depth[j]<element) j--;
	mov	edi, ebx
	shl	edi, 1
	add	edi, dword ptr [esi.s3d_depths]
@@small2:
	cmp	word ptr [edi], dx
	jge	@@break_small2
	dec	ebx
	sub	edi, 2
	jmp	@@small2
@@break_small2:

	; if(i<=j)
	cmp	eax, ebx
	jg	@@skip_xchg

	mov	edi, dword ptr [esi.s3d_depths]
	mov	cx, word ptr [edi + eax*2]
	xchg	cx, word ptr [edi + ebx*2]
	mov	word ptr [edi + eax*2], cx
	
	mov	edi, dword ptr [esi.s3d_order]
	mov	cx, word ptr [edi + eax*2]
	xchg	cx, word ptr [edi + ebx*2]
	mov	word ptr [edi + eax*2], cx

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
	mov	edi, dword ptr [esi.s3d_order]
	mov	ecx, dword ptr [esi.s3d_vis_faces]
	or	ecx, ecx
	jnz	@@draw
	ret
@@draw:
	push	edi
	push	ecx

	movzx	edi, word ptr [edi]
	mov	edx, edi
	mov	eax, edi
	shl	edi, 1
	shl	eax, 2
	add	edi, eax
	add	edi, dword ptr [esi.s3d_faces]

	movzx	ebx, word ptr [edi.face_v1]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	movsx	eax, ax
	mov	t_x1, eax
	mov	ax, word ptr [ebx.y2d]
	movsx	eax, ax
	mov	t_y1, eax

	movzx	ebx, word ptr [edi.face_v2]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	movsx	eax, ax
	mov	t_x2, eax
	mov	ax, word ptr [ebx.y2d]
	movsx	eax, ax
	mov	t_y2, eax

	movzx	ebx, word ptr [edi.face_v3]
	shl	ebx, 2
	add	ebx, dword ptr [esi.s3d_t_points]
	mov	ax, word ptr [ebx.x2d]
	movsx	eax, ax
	mov	t_x3, eax
	mov	ax, word ptr [ebx.y2d]
	movsx	eax, ax
	mov	t_y3, eax


	;mov	eax, d [CurrLightModel]
	;cmp	eax, WF_DRAW_FLAT
	mov	al, b [FlatFlag]
	cmp	al, 1
	je	rs3d_flt

	mov	eax, 1
	mov	ebx, edx
	call	d [CurrColorProc]
	mov	ebx, edx
	and	ebx, 2 ;3
	shl	ebx, 6
	add	ebx, eax
	mov	d [GrdCol1], ebx

	mov	eax, 2
	mov	ebx, edx
	call	d [CurrColorProc]
	mov	ebx, edx
	and	ebx, 2 ;3
	shl	ebx, 6
	add	ebx, eax
	mov	d [GrdCol2], ebx

	mov	eax, 3
	mov	ebx, edx
	call	d [CurrColorProc]
	mov	ebx, edx
	and	ebx, 2 ;3
	shl	ebx, 6
	add	ebx, eax
	mov	d [GrdCol3], ebx

	mov	eax, d [t_x1]
	mov	d [GrdX1], eax
	mov	eax, d [t_y1]
	mov	d [GrdY1], eax
	mov	eax, d [t_x2]
	mov	d [GrdX2], eax
	mov	eax, d [t_y2]
	mov	d [GrdY2], eax
	mov	eax, d [t_x3]
	mov	d [GrdX3], eax
	mov	eax, d [t_y3]
	mov	d [GrdY3], eax

	call	GouraudTriangle

	jmp	@@next_face

rs3d_flt:
	mov	eax, 1
	mov	ebx, edx
	call	d [CurrColorProc]

	and	edx, 2 ;3
	shl	edx, 6
	add	edx, eax
	mov	d [t_col], edx

	call	flat_triangle

@@next_face:
	pop	ecx
	pop	edi
	add	edi, 2
	dec	ecx
	jnz	@@draw

	ret
endp

comment #
;------------------------------------------------------------
;	in:	esi - offset to struct3d
;	out:	none
;------------------------------------------------------------

init_face_normals proc
	mov	ebp, esi

	mov	eax, dword ptr [ebp.s3d_faces]
	mov	edx, dword ptr [ebp.s3d_face_nrm]
	mov	ecx, dword ptr [ebp.s3d_n_faces_]
@@get_n:
	movzx	esi, word ptr [eax.face_v1]
	mov	ebx, esi
	shl	esi, 2
	shl	ebx, 3
	add	esi, ebx
	add	esi, dword ptr [ebp.s3d_points]

	movzx	edi, word ptr [eax.face_v2]
	mov	ebx, edi
	shl	edi, 2
	shl	ebx, 3
	add	edi, ebx
	add	edi, dword ptr [ebp.s3d_points]

	mov	ebx, offset uv
	call	MakeVector

	mov	esi, edi

	movzx	edi, word ptr [eax.face_v3]
	mov	ebx, edi
	shl	edi, 2
	shl	ebx, 3
	add	edi, ebx
	add	edi, dword ptr [ebp.s3d_points]

	mov	ebx, offset fv
	call	MakeVector

	mov	esi, offset uv
	mov	edi, offset fv
	mov	ebx, edx
	call	CrossProduct

	mov	edi, edx
	call	NormalizeVector

	add	eax, size face
	add	edx, size vector3d
	dec	ecx
	jnz	@@get_n
	ret
endp #

min_d	dd 500.0 ;350.0 ;400.0
max_d	dd 4200.0


uv	vector3d ?
fv	vector3d ?
rv	vector3d ?

f_x1	dw ?
f_y1	dw ?
f_x2	dw ?
f_y2	dw ?
f_x3	dw ?
f_y3	dw ?
