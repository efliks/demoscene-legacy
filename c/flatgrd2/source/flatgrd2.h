/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

#ifndef _flatgrd_h
#define _flatgrd_h

/* ------------------------------ constants ------------------------------ */

#define MAX_DEGS 1024
#define PERSPECTIVE 256.0
#define CORRECT_X 160.0
#define CORRECT_Y 100.0

#define COLOR_EMPTY 0xff
#define FLAT_SHADING 0
#define GOURAUD_SHADING 1
#define ENVIRONMENT_MAPPING 2
#define ENV_BUMP 3

#define BMP_HEADER_SIZE 54
#define PCX_HEADER_SIZE 128

#define MAX_PHONG_COLORS 63
#define PHONG_DELTA_ANGLE (M_PI/2)/MAX_PHONG_COLORS
#define PHONG_SPEC_EXPONENT 12

#define TEXT_COLOR MAX_PHONG_COLORS+1
#define DELAY 0
#define NO_DELAY 1

#define SHIFT_CONST 8

#define BUFFER_SIZE 64000

/* ------------------------------ data types ------------------------------ */

typedef float MATRIX[3][3];

typedef struct
{
	float x,y,z;
}POINT3D,VECTOR3D;

typedef struct
{
	int x,y;
}POINT2D;

typedef struct
{
	short v1,v2,v3;
}FACE;

typedef struct
{
	int x1,y1,x2,y2,x3,y3;
}tri_struct;

typedef struct
{
	short n_points;
	short n_faces;
	short angle_x,angle_y,angle_z;
	short visible_faces;
	MATRIX object_matrix;

	POINT3D	*points,*rotated_points;
	POINT2D *translated_points;
	VECTOR3D *point_normals,*rotated_point_normals;
	FACE *faces;
	VECTOR3D *face_normals;
	short int *face_depth,*order_table;

	tri_struct *tex_coords;

}OBJECT3D;

/* ------------------------------ global variables ------------------------------ */

extern float sintab[MAX_DEGS],costab[MAX_DEGS];
extern char frame_file[16];
extern long bytes_written;
extern unsigned char *envmap, *bumpmap;
extern unsigned char *frame_buffer,palette[768];

/* ------------------------------ 3dmath.c ------------------------------ */

void init_sincos(void);

void translate_point(POINT2D *dest,POINT3D *src);
void rotate_point(POINT3D *dest,POINT3D *src,
	short angle_x,short angle_y,short angle_z);

void mx_identity(MATRIX m);
void mx_mul(MATRIX result,MATRIX a,MATRIX b);
void mx_rotation_matrix(MATRIX result,short ax,short ay,short az);
void mx_rotate_points(POINT3D *dest_tab,POINT3D *src_tab,
	int n_points,MATRIX matrix);

void normalize_vector(VECTOR3D *v);
void cross_product(VECTOR3D *result,VECTOR3D *v1,VECTOR3D *v2);
float dot_product(VECTOR3D *v1,VECTOR3D *v2);

/* ------------------------------ 3dobj.c ------------------------------ */

OBJECT3D *load_object3d(char *filename);
void unload_object3d(OBJECT3D *obj);
void init_face_normals(OBJECT3D *obj);
void init_point_normals(OBJECT3D *obj);
void rotate_translate_object3d(OBJECT3D *obj,
			int delta_x,int delta_y,int delta_z);
void draw_object3d(OBJECT3D *obj,VECTOR3D *light,int drawing_mode);
void sort_faces(OBJECT3D *obj);
void depth_sorting(OBJECT3D *obj,short int left,short int right);

void init_tex_coords(OBJECT3D *o);
void save_tex_coords(OBJECT3D *o, char *filename);

/* ------------------------------ dump_src.c ------------------------------ */

long dump_pcx(char *filename);
long dump_bmp(char *filename);
int save_frame(void);

/* ------------------------------ envmap.c ------------------------------ */

int enable_envmap(void);
void disable_envmap(void);

/* ------------------------------ bumpmap.c ------------------------------ */

int enable_bumpmap(void);
void disable_bumpmap(void);

/* ------------------------------ font.c ------------------------------ */

void init_font(void);
void put_char(char ascii_code,int x,int y,
		unsigned char color,unsigned char *where);
void put_string(char *text,int x,int y,
		unsigned char color,unsigned char *where);

/* ------------------------------ graph.c ------------------------------ */

void clear_buffer(void);
void copy_buffer(void);
void retrace(void);
void set_palette(void);
void get_palette(void);

/* ------------------------------ grd_tri.c ------------------------------ */

void gouraud_line(int x1,int x2,int y,unsigned char c1,unsigned char c2,
				unsigned char *buffer);
void gouraud_triangle(int x1,int y1,int x2,int y2,int x3,int y3,
				unsigned char c1,unsigned char c2,unsigned char c3,
				unsigned char *buffer);

/* ------------------------------ tex_tri.c ------------------------------ */

void textured_line(int x1,int x2,int y,
			int tx1,int ty1,int tx2,int ty2,unsigned char *texture,
			unsigned char *buffer);
void textured_triangle(int x1,int y1,int x2,int y2,int x3,int y3,
						int tx1,int ty1,int tx2,int ty2,int tx3,int ty3,
						unsigned char *texture,unsigned char *buffer);

/* ------------------------------ flat_tri.c ------------------------------ */

void flat_line(int x1,int x2,int y,unsigned char color,unsigned char *buffer);
void flat_triangle(int x1,int y1,int x2,int y2,int x3,int y3,
				unsigned char color,unsigned char *buffer);

/* ------------------------------ bump_tri.c ------------------------------ */

void bumpmapped_triangle(tri_struct *tri, tri_struct *bump_tri,
						tri_struct *env_tri,
						unsigned char *bumpmap, unsigned char *envmap,
						unsigned char *buffer);
void bump_line(int x1,int x2,int y,
				int bx1,int by1,int bx2,int by2,
				int ex1,int ey1,int ex2,int ey2,
				unsigned char *bumpmap, unsigned char *envmap,
				unsigned char *buffer);

/* ------------------------------ phongpal.c ------------------------------ */

void random_phong_palette(void);
int load_palette(char *filename);
void save_palette(char *filename);
void make_font_palette(void);

#endif