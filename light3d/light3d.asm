;	Mikolaj Felix 21/06/2001
;	mfelix@polbox.com

.model tiny
.386
.code
org 100h

start:
	call alloc_screen_buffer

	call init_sincos
	call init_face_normals
	call init_point_normals

	push word ptr 0
	push word ptr 0
	push word ptr 0
	push word ptr -100
	push word ptr 0
	push word ptr 0
	call set_light

	mov ax,13h
	int 10h

	mov dx,03c8h
	xor ax,ax
	out dx,al
	inc dx
	mov cx,64
set_color:
	out dx,al
	out dx,al
	out dx,al
	inc al
	dec cx
	jnz set_color

main_loop:

	; ---------- rotate object ----------

	mov si,offset object
	mov di,offset object_rotated
	mov cx,MAX_POINTS*3
	cld
	rep movsd

	mov si,offset angle_x
	mov di,offset object_rotated
	mov cx,MAX_POINTS
	call rotate_points

	; ---------- rotate point-normals ----------

	mov si,offset point_normals
	mov di,offset rotated_point_normals
	mov cx,MAX_POINTS*3
	cld
	rep movsd

	mov si,offset angle_x
	mov di,offset rotated_point_normals
	mov cx,MAX_POINTS
	call rotate_points
	
	; ---------- translate object ----------

	mov si,offset object_rotated
	mov di,offset object_translated
	mov cx,MAX_POINTS
	call translate_points

	; ---------- setup light ----------

	mov si,offset light
	mov di,offset light_rotated
	mov cx,3
	rep movsd

	; ---------- draw gouraud shading ----------
	
	call draw_object

	call wait_for_frame
	call copy_screen_buffer
	call clear_screen_buffer

	add angle_x,1
	add angle_y,3
	add angle_z,1

	in al,60h
	dec al
	jnz main_loop

	mov ax,03h
	int 10h

	call free_screen_buffer
	mov ah,4ch
	int 21h

draw_object proc
	
@@x1 equ word ptr [bp-2]
@@y1 equ word ptr [bp-4]
@@x2 equ word ptr [bp-6]
@@y2 equ word ptr [bp-8]
@@x3 equ word ptr [bp-10]
@@y3 equ word ptr [bp-12]
@@x4 equ word ptr [bp-14]
@@y4 equ word ptr [bp-16]
@@c1 equ word ptr [bp-18]
@@c2 equ word ptr [bp-20]
@@c3 equ word ptr [bp-22]
@@c4 equ word ptr [bp-24]
@@fstatus equ word ptr [bp-26]

	push bp
	mov bp,sp
	sub sp,26

	mov si,offset faces
	mov cx,MAX_FACES
@@draw_face:
	push cx

	mov di,bp
	sub di,2
	mov cx,4
@@get_vertex:
	movzx bx,byte ptr [si]
	shl bx,2
	mov ax,word ptr object_translated[bx]
	mov word ptr ss:[di],ax
	mov ax,word ptr object_translated[bx+2]
	mov word ptr ss:[di-2],ax
	inc si
	sub di,4
	dec cx
	jnz @@get_vertex

	; ---------- calculate z-normal (2D) ----------

	mov ax,@@y1
	sub ax,@@y3
	mov bx,@@x2
	sub bx,@@x1
	imul bx
	shl edx,16
	mov dx,ax
	push edx
	mov ax,@@x1
	sub ax,@@x3
	mov bx,@@y2
	sub bx,@@y1
	imul bx
	shl edx,16
	mov dx,ax
	pop ebx
	sub ebx,edx			; normal_z = (y1-y3)*(x2-x1)-(x1-x3)*(y2-y1)
	or ebx,ebx
	jl @@next_face			; normal_z < 0

	; ---------- draw polygon ----------

	mov cx,4
	mov di,bp
	sub di,18			; di is a pointer to color1
	sub si,4
@@get_color:
	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax

	; ---------- make dot product ----------

	fld dword ptr rotated_point_normals[bx]
	fmul dword ptr light_rotated
	fld dword ptr rotated_point_normals[bx+4]
	fmul dword ptr light_rotated[4]
	faddp st(1),st
	fld dword ptr rotated_point_normals[bx+8]
	fmul dword ptr light_rotated[8]
	faddp st(1),st

	fcom cos_min
	fstsw @@fstatus
	mov ah,byte ptr @@fstatus[1]
	sahf
	jb @@set_color_to_zero
	fcom cos_max
	fstsw @@fstatus
	mov ah,byte ptr @@fstatus[1]
	sahf
	ja @@set_color_to_zero

	fmul max_gouraud_colors
	fistp word ptr ss:[di]

	jmp @@next_color

@@set_color_to_zero:
	ffree st
	mov word ptr ss:[di],0
@@next_color:
	inc si
	sub di,2
	dec cx
	jnz @@get_color

	; ---------- push arguments ----------

	mov di,bp
	sub di,24
	mov cx,12			; 12 words
@@push_arg:
	push word ptr ss:[di]
	add di,2
	dec cx
	jnz @@push_arg

COMMENT #
	push @@c4
	push @@c3
	push @@c2
	push @@c1
	push @@y4
	push @@x4
	push @@y3
	push @@x3
	push @@y2
	push @@x2
	push @@y1
	push @@x1 #

	call draw_grds_polygon

@@next_face:
	pop cx
	dec cx
	jnz @@draw_face

	mov sp,bp
	pop bp
	ret
endp

set_light proc

@@x1 equ word ptr [bp+4]
@@y1 equ word ptr [bp+6]
@@z1 equ word ptr [bp+8]
@@x2 equ word ptr [bp+10]
@@y2 equ word ptr [bp+12]
@@z2 equ word ptr [bp+14]
@@lenght equ dword ptr [bp-4]

	push bp
	mov bp,sp
	sub sp,4
	
	mov ax,@@x1
	sub @@x2,ax
	mov ax,@@y1
	sub @@y2,ax
	mov ax,@@z1
	sub @@z2,ax

	fild @@x2
	fimul @@x2
	fild @@y2
	fimul @@y2
	faddp st(1),st
	fild @@z2
	fimul @@z2
	faddp st(1),st
	fsqrt
	fstp @@lenght

	fild @@x2
	fdiv @@lenght
	fstp dword ptr light
	fild @@y2
	fdiv @@lenght
	fstp dword ptr light[4]
	fild @@z2
	fdiv @@lenght
	fstp dword ptr light[8]

	mov sp,bp
	pop bp
	ret 12
endp

init_face_normals proc

@@normal_x equ dword ptr [bp-4]
@@normal_y equ dword ptr [bp-8]
@@normal_z equ dword ptr [bp-12]
@@lenght equ dword ptr [bp-16]

	push bp
	mov bp,sp
	sub sp,16

	mov si,offset faces
	mov di,offset face_normals
	mov cx,MAX_FACES
@@get_normal:

	; ---------- compute x-normal ----------

	; x_normal=(v2.y-p1.y)(v1.z-p3.z)-(v2.z-p1.z)(v1.y-v3.y)

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+4]		; object[faces[i].v2].y

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+4]		; -object[faces[i].v1].y

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+8]		; object[faces[i].v1].z

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+8]		; -object[faces[i].v3].z

	fmulp st(1),st
	fstp @@normal_x

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+8]		; object[faces[i].v2].z

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+8]		; -object[faces[i].v1].z

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+4]		; object[faces[i].v1].y

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+4]		; -object[faces[i].v3].y

	fmulp st(1),st
	fsubr @@normal_x
	fstp @@normal_x

	; ---------- compute y-normal ----------

	; y_normal=(v2.z-v1.z)(v1.x-v3.x)-(v2.x-v1.x)(v1.z-v3.z)

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+8]		; object[faces[i].v2].z

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+8]		; -object[faces[i].v1].z

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx]		; object[faces[i].v1].x

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx]		; -object[faces[i].v3].x

	fmulp st(1),st
	fstp @@normal_y

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx]		; object[faces[i].v2].x

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx]		; -object[faces[i].v1].x

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+8]		; object[faces[i].v1].z

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+8]		; -object[faces[i].v3].z

	fmulp st(1),st
	fsubr @@normal_y
	fstp @@normal_y

	; ---------- compute z-normal ----------

	; z_normal=(v2.x-v1.x)(v1.y-v3.y)-(v2.y-v1.y)(v1.x-v3.x)

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx]		; object[faces[i].v2].x

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx]		; -object[faces[i].v1].x

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+4]		; object[faces[i].v1].y

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+4]		; -object[faces[i].v3].y

	fmulp st(1),st
	fstp @@normal_z

	movzx bx,byte ptr [si+1]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx+4]		; object[faces[i].v2].y

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx+4]		; -object[faces[i].v1].y

	movzx bx,byte ptr [si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld dword ptr object[bx]		; object[faces[i].v1].x

	movzx bx,byte ptr [si+2]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fsub dword ptr object[bx]		; -object[faces[i].v3].x

	fmulp st(1),st
	fsubr @@normal_z
	fstp @@normal_z

	; ---------- get vector lenght ----------

	fld @@normal_x
	fmul @@normal_x
	fld @@normal_y
	fmul @@normal_y
	faddp st(1),st
	fld @@normal_z
	fmul @@normal_z
	faddp st(1),st
	fsqrt
	fstp @@lenght

	; ---------- make unit-vector ----------
	
	fld @@normal_x
	fdiv @@lenght
	fstp dword ptr [di]		; vector x

	fld @@normal_y
	fdiv @@lenght
	fstp dword ptr [di+4]		; vector y

	fld @@normal_z
	fdiv @@lenght
	fstp dword ptr [di+8]		; vector z

	add si,4
	add di,12
	dec cx
	jnz @@get_normal

	mov sp,bp
	pop bp
	ret
endp

init_point_normals proc
	
@@x equ dword ptr [bp-4]
@@y equ dword ptr [bp-8]
@@z equ dword ptr [bp-12]
@@which_face equ byte ptr ss:[bp-24]
@@point_number equ word ptr [bp-26]
@@temp equ word ptr [bp-28]

	push bp
	mov bp,sp
	sub sp,28

	mov di,offset point_normals
	mov @@point_number,0
@@point:

	; ---------- get faces in which point is included ----------

	mov si,offset faces
	xor bx,bx			; offset in which_face / number of faces
	xor dx,dx			; face number
@@face:
	mov cx,4
@@check_vertex:
	xor ah,ah
	lodsb
	cmp @@point_number,ax
	jne @@next_vertex

	push si
	mov si,bx
	mov @@which_face[si],dl
	pop si
	inc bx

@@next_vertex:
	dec cx
	jnz @@check_vertex

	inc dx
	cmp dx,MAX_FACES
	jne @@face

	; ---------- sum up the normals ----------

	mov dx,bx			; number of normals

	mov @@x,0
	mov @@y,0
	mov @@z,0

	xor si,si
@@sum_normals:
	movzx bx,byte ptr @@which_face[si]
	mov ax,bx
	shl bx,3
	shl ax,2
	add bx,ax
	fld @@x
	fadd dword ptr face_normals[bx]
	fstp @@x
	fld @@y
	fadd dword ptr face_normals[bx+4]
	fstp @@y
	fld @@z
	fadd dword ptr face_normals[bx+8]
	fstp @@z
	inc si
	cmp si,dx
	jne @@sum_normals

	; ---------- get average values ----------

	mov @@temp,dx
	fild @@temp
	fld st(0)
	fld st(0)
	fdivr @@x
	fstp @@x
	fdivr @@y
	fstp @@y
	fdivr @@z
	fstp @@z

	; ---------- make unit-vector ----------

	fld @@x
	fmul @@x
	fld @@y
	fmul @@y
	faddp st(1),st
	fld @@z
	fmul @@z
	faddp st(1),st
	fsqrt
	fld st(0)
	fld st(0)

	fdivr @@x
	fstp dword ptr [di]		; vector x
	fdivr @@y
	fstp dword ptr [di+4]		; vector y
	fdivr @@z
	fstp dword ptr [di+8]		; vector z

	add di,12
	inc @@point_number
	cmp @@point_number,MAX_POINTS
	jne @@point

	mov sp,bp
	pop bp
	ret
endp

include math3d_2.asm
include graphlib.asm
include grds_pol.asm

MAX_POINTS equ 8
MAX_FACES equ 6

angle_x dw 0
angle_y dw 0
angle_z dw 0

max_gouraud_colors dd 63.0
cos_min dd 0.0
cos_max dd 1.0

object	dd -50.0,-50.0,50.0, 50.0,-50.0,50.0, 50.0,50.0,50.0, -50.0,50.0,50.0	
	dd -50.0,-50.0,-50.0, 50.0,-50.0,-50.0, 50.0,50.0,-50.0, -50.0,50.0,-50.0

faces db 0,1,2,3, 5,4,7,6, 1,5,6,2, 4,0,3,7, 4,5,1,0, 3,2,6,7

object_rotated dd MAX_POINTS dup(?,?,?)
object_translated dw MAX_POINTS dup(?,?)

face_normals dd MAX_FACES dup(?,?,?)
point_normals dd MAX_POINTS dup(?,?,?)
rotated_point_normals dd MAX_POINTS dup(?,?,?)

light dd 3 dup(?)
light_rotated dd 3 dup(?)

end start