#ifndef _3dmath_h
#define _3dmath_h

#define MAX_DEGS 512

#define PERSPECTIVE 256.0
#define CORRECT_X 160.0
#define CORRECT_Y 100.0

#define sine sintab
#define cosine costab
#define VECTOR VECTOR3D
#define m3x3 MATRIX

/* global variables */

extern float sintab[MAX_DEGS],costab[MAX_DEGS];

/* data types */

typedef float MATRIX[3][3];

typedef struct
{
	float x,y,z;
}POINT3D,VECTOR3D;

typedef struct
{
	short int x,y;
}POINT2D;

typedef struct
{
	short v1,v2,v3;
}FACE;

/* routines */

void init_sincos(void);

void translate_point(POINT2D *dest,POINT3D *src);
void rotate_point(POINT3D *dest,POINT3D *src,
	unsigned angle_x,unsigned angle_y,unsigned angle_z);

void mx_identity(MATRIX m);
void mx_mul(MATRIX result,MATRIX a,MATRIX b);
void mx_rotation_matrix(MATRIX result,unsigned ax,unsigned ay,unsigned az);
void mx_rotate_points(POINT3D *dest_tab,POINT3D *src_tab,
	int n_points,MATRIX matrix);

void normalize_vector(VECTOR3D *v);
void cross_product(VECTOR3D *result,VECTOR3D *v1,VECTOR3D *v2);
float dot_product(VECTOR3D *v1,VECTOR3D *v2);

#endif