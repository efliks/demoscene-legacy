#ifndef _letgen_h
#define _letgen_h

typedef struct
{
	float x, y, z;
}vertex3d;

typedef struct
{
	short v1, v2, v3;
}face;

typedef struct
{
	long n_vertices;
	long n_faces;
	long res1;
	vertex3d *vertices;
	long res2, res3;
	face *faces;
}tiny_struct3d;


extern void MakeLetter3d(long ascii_code);
extern tiny_struct3d *EnableBuffer3d(void);

#endif
