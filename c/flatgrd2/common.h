#ifndef _COMMON_H
#define _COMMON_H

#define SHIFT_CONST 8

#define MAX_PHONG_COLORS 63
#define PHONG_DELTA_ANGLE (M_PI / 2) / MAX_PHONG_COLORS
#define PHONG_SPEC_EXPONENT 12

#define COLOR_EMPTY 0xff

#define FLAT_SHADING 0
#define GOURAUD_SHADING 1
#define ENVIRONMENT_MAPPING 2
#define ENV_BUMP 3

typedef struct
{
    int x1, y1, x2, y2, x3, y3;
} tri_struct;

#endif  // _COMMON_H
