#ifndef _BUMP_TRI_H
#define _BUMP_TRI_H

#include "common.h"


void bumpmapped_triangle(tri_struct* tri, tri_struct* bump_tri,
    tri_struct* env_tri,
    unsigned char* bumpmap, unsigned char* envmap,
    unsigned char* buffer);

#endif  // _BUMP_TRI_H
