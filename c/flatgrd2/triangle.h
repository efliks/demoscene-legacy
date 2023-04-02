#ifndef _TRIANGLES_H
#define _TRIANGLES_H

void flat_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char color, unsigned char* buffer);

void gouraud_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char c1, unsigned char c2, unsigned char c3,
    unsigned char* buffer);

void textured_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    int tx1, int ty1, int tx2, int ty2, int tx3, int ty3,
    unsigned char* texture, unsigned char* buffer);

void bumpmapped_triangle(tri_struct* tri, tri_struct* bump_tri,
    tri_struct* env_tri,
    unsigned char* bumpmap, unsigned char* envmap,
    unsigned char* buffer);

#endif  // _TRIANGLES_H
