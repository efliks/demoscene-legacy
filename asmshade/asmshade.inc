MAX_DEGS EQU 512

SHIFT_CONST equ 12

TEXT_COLOR equ 64

FLAT_SHADING equ 0
GOURAUD_SHADING equ 1
ENV_MAPPING equ 2

;------------------------------------------------------------

point3d struc
	x3d dd ?
	y3d dd ?
	z3d dd ?
ends

point2d struc
	x2d dw ?
	y2d dw ?
ends

vector3d struc
	vec_x dd ?
	vec_y dd ?
	vec_z dd ?
ends

face struc
	face_v1 dw ?
	face_v2 dw ?
	face_v3 dw ?
ends

matrix struc
	dd 9 dup(?)
ends