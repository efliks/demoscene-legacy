#ifndef _OBJ3D_H
#define _OBJ3D_H

#include "common.h"
#include "3dmath.h"


typedef struct
{
    short v1, v2, v3;
} FACE;

typedef struct
{
    short n_points;
    short n_faces;
    short angle_x, angle_y, angle_z;
    short visible_faces;
    MATRIX object_matrix;

    POINT3D *points, *rotated_points;
    POINT2D* translated_points;
    VECTOR3D *point_normals, *rotated_point_normals;
    FACE* faces;
    VECTOR3D* face_normals;
    short int *face_depth, *order_table;

    tri_struct* tex_coords;

} OBJECT3D;


OBJECT3D* load_object3d(char* filename);
void unload_object3d(OBJECT3D* obj);

void rotate_translate_object3d(OBJECT3D* obj, int delta_x, int delta_y, int delta_z);

void draw_object3d(OBJECT3D* obj, VECTOR3D* light, int drawing_mode);

#endif  // _3DOBJ_H
