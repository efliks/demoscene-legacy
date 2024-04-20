; math3d

MAX_DEGS equ 512

point2d struc
   x2d dw ?
   y2d dw ?
ends

point3d struc
   x3d dd ?
   y3d dd ?
   z3d dd ?
ends

vector3d struc
   vec_x dd ?
   vec_y dd ?
   vec_z dd ?
ends

matrix struc
   dd 9 dup(?)
ends

global init_sincos:PROC

global mx_rotation_matrix:PROC
global mx_rotate_points:PROC

global translate_points:PROC

global make_vector:PROC
global normalize_vector:PROC

global dot_product:PROC
global cross_product:PROC

global rotate_points_old:PROC

global sintab:DWORD
global costab:DWORD
