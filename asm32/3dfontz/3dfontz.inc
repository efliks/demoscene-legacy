b	equ byte ptr
w	equ word ptr
d	equ dword ptr
q	equ qword ptr
o	equ offset

;------------------------------------------------------------

REQ_MEM		equ 64000 + SINCOS_SIZE + 768 + 64000
LETTER_TIME	equ 900
TEXT_COLOR	equ 50

; 3dmath constants
VERTEX_OUT	equ -100
MAX_DEGS	equ 1024

SINLOOKUP	equ 0
COSLOOKUP	equ SINLOOKUP + MAX_DEGS*4
SINCOS_SIZE	equ COSLOOKUP + MAX_DEGS*4

; gouraud triangle
GT_SHIFT	equ 12

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

; shadow line
SHADOW_X	equ -40
SHADOW_Y	equ 40

; move3d
M_SPEED		equ 0.5

;------------------------------------------------------------

; 3d math structs
point3d struc
	x3d	dd ?
	y3d	dd ?
	z3d	dd ?
ends

point2d struc
	x2d	dw ?
	y2d	dw ?
ends

vector3d struc
	vec_x	dd ?
	vec_y	dd ?
	vec_z	dd ?
ends

matrix struc
	m_00	dd ?
	m_01	dd ?
	m_02	dd ?
	m_10	dd ?
	m_11	dd ?
	m_12	dd ?
	m_20	dd ?
	m_21	dd ?
	m_22	dd ?
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

;------------------------------------------------------------

; used by move3d
move3d_struct struc
	m3d_StartX	dd ?
	m3d_StartY	dd ?
	m3d_StartZ	dd ?

	m3d_DestX	dd ?
	m3d_DestY	dd ?
	m3d_DestZ	dd ?
ends
