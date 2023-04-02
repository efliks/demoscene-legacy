#ifndef _3DMATH_H
#define _3DMATH_H

#define MAX_DEGS        1024
#define CORRECT_X       160.0
#define CORRECT_Y       100.0
#define PERSPECTIVE     256.0


typedef float MATRIX[3][3];

typedef struct
{
    float x, y, z;
} POINT3D, VECTOR3D;

typedef struct
{
    int x, y;
} POINT2D;


void init_sincos(void);

void translate_point(POINT2D* dest, POINT3D* src);

void mx_rotation_matrix(MATRIX result, short ax, short ay, short az);
void mx_rotate_points(POINT3D* dest_tab, POINT3D* src_tab, int n_points, MATRIX matrix);

void normalize_vector(VECTOR3D* v);

void cross_product(VECTOR3D* result, VECTOR3D* v1, VECTOR3D* v2);
float dot_product(VECTOR3D* v1, VECTOR3D* v2);

#endif
