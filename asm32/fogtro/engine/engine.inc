
; struct3d
S3D_N_VERTS		equ 0
S3D_N_FACES		equ 4
S3D_VISABLE_FACES	equ 8

S3D_PTR_B_VERTS		equ 12
S3D_PTR_R_VERTS		equ 16
S3D_PTR_T_VERTS		equ 20

S3D_PTR_FACES		equ 24
S3D_PTR_DEPTH		equ 28
S3D_PTR_ORDER		equ 32

;S3D_PTR_B_NORMALS	equ 36
;S3D_PTR_R_NORMALS	equ 40
S3D_PTR_FACE_PARAMS	equ 36

STRUCT3D_SIZE		equ 44

;************************************************************

; camera
CAM_POS			equ 0
CAM_TARGET		equ 12
CAM_ROLL		equ 24
CAM_MATRIX		equ 28
CAM_PTR_TRACK		equ 28 + 9 * 4
CAM_TRACK_LEN		equ CAM_PTR_TRACK + 4
CAM_TRACK_POS		equ CAM_TRACK_LEN + 4
CAM_FLAGS		equ CAM_TRACK_POS + 4

CAMERA_SIZE		equ CAM_FLAGS + 4


CF_NO_TARGET		equ 1
CF_WITH_TARGET		equ 2

;************************************************************

; object3d
O_PTR_STRUCT3D		equ 0
O_MATRIX		equ O_PTR_STRUCT3D + 4
O_ANGLE_X		equ O_MATRIX + 9 * 4
O_ANGLE_Y		equ O_ANGLE_X + 4
O_ANGLE_Z		equ O_ANGLE_Y + 4
O_ADD_ANGLE_X		equ O_ANGLE_Z + 4
O_ADD_ANGLE_Y		equ O_ADD_ANGLE_X + 4
O_ADD_ANGLE_Z		equ O_ADD_ANGLE_Y + 4
O_POS			equ O_ADD_ANGLE_Z + 4
O_FLAGS			equ O_POS + 12

OBJECT3D_SIZE		equ O_FLAGS + 4


OF_STATIC		equ 1
OF_MOVEABLE		equ 2

;************************************************************

; world
W_PTR_STRUCT3D		equ 0
W_PTR_CAMERA		equ 4
W_NUM_OF_OBJECTS	equ 8
W_PTR_OBJECTS		equ 12
W_FLAGS			equ 16

WORLD_SIZE		equ 20


WF_DRAW_FLAT		equ 1
WF_DRAW_GOURAUD		equ 2

;************************************************************

; track (used in older engine, currently without any sense ;)
T_NUM_OF_VERTS		equ 0
T_PTR_VERTS		equ 4
T_CURR_POSITION		equ 8

TRACK_SIZE		equ 12


T_STD_NUM_OF_VERTS	equ MAX_DEGS

;************************************************************

; face params
FP_PTR_TEXTURE		equ 0
FP_X1			equ 4
FP_Y1			equ 5
FP_X2			equ 6
FP_Y2			equ 7
FP_X3			equ 8
FP_Y3			equ 9

FACE_PARAMS_SIZE	equ 10

;************************************************************

MOVE_PAL_COLOR		equ 12
MIN_COLOR		equ 1
