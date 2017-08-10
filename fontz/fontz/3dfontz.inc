b	equ byte ptr
w	equ word ptr
d	equ dword ptr
q	equ qword ptr
o	equ offset

;------------------------------------------------------------

; 3d letters generator constants
N_POINTS	equ 8 * 64
N_FACES		equ 12 * 64
BUFFER3D_SIZE	equ N_POINTS * (size point3d) + N_FACES * (size face)

CSIZE		equ 32.0
LETGEN_SCALEX	equ 0.3
LETGEN_SCALEY	equ 0.3
LETGEN_SCALEZ	equ 0.6

POLYS_EQUAL	equ 1
POLYS_DIFF	equ -1


;------------------------------------------------------------

; 3d math structs
point3d struc
	x3d	dd ?
	y3d	dd ?
	z3d	dd ?
ends

;------------------------------------------------------------

; engine structs
struct3d struc
	s3d_n_points	dd ?			; number of points
	s3d_n_faces	dd ?			; number of faces
	s3d_vis_faces	dd ?			; number of visible faces

	s3d_points	dd ?			; base points
	s3d_r_points	dd ?			; rotated points
	s3d_t_points	dd ?			; 2d points

	s3d_faces	dd ?			; faces
	s3d_depths	dd ?			; faces' depths
	s3d_order	dd ?			; order table

	s3d_point_nrm	dd ?			; point normals
	s3d_r_point_nrm	dd ?			; rotated point normals


	s3d_AngleX	dd ?
	s3d_AngleY	dd ?
	s3d_AngleZ	dd ?

	s3d_DeltaX	dd ?
	s3d_DeltaY	dd ?
	s3d_DeltaZ	dd ?

	s3d_MovingTime	dd ?
	s3d_CurrTime	dd ?

	s3d_Movement	dd ?
ends

face struc
	face_v1		dw ?
	face_v2		dw ?
	face_v3		dw ?
ends

;------------------------------------------------------------

; used by letters generator
tiny_struct3d struc
	ts3d_n_points	dd ?
	ts3d_n_faces	dd ?
			dd ?
	ts3d_points	dd ?
			dd ?
			dd ?
	ts3d_faces	dd ?
ends

polygon struc
	poly_v1		dw ?
	poly_v2		dw ?
	poly_v3		dw ?
	poly_v4		dw ?
ends
