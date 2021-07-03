#ifndef _mrt_h
#define _mrt_h



//------------------------------------------------------------

#define VERSION "1.1"


#ifndef M_PI
#define M_PI		3.1415927
#endif


#define C_EMPTY		0
#define C_WHITE		0
#define C_RED		32
#define C_GREEN		64
#define C_BLUE		96
#define C_YELLOW	128
#define C_CYAN		160
#define C_MAGENTA	192
#define C_BLUE2		224

#define MAX_LINES		30
#define MAX_LINE_LEN	256
#define MAX_NAME_LEN	16
#define MAX_NUM_LEN		16
#define MAX_FILENAME_LEN 200
#define MAX_LABEL_LEN	10
#define MAX_VAL_LEN		64

#define MAX_DIST		60.0
#define ANGLE_OF_VIEW	70.0

#define ERR_OK			 0
#define ERR_NOFILE		-1
#define ERR_COMMENT		-2
#define ERR_CHARS		-4
#define ERR_COMMENT2	-5
#define ERR_EMPTY		-6
#define ERR_NAMECHAR	-7
#define ERR_NAMELEN		-8
#define ERR_NONAME		-9
#define ERR_BADNAME		-10
#define ERR_NUMLEN		-11
#define ERR_NUM			-12
#define ERR_LINEEND		-13
#define ERR_NUMARGS		-14
#define ERR_CAMERA		-15
#define ERR_LIGHT		-16
#define ERR_SCRMEM		-17
#define ERR_SPHMEM		-18
#define ERR_PLNMEM		-19
#define ERR_RES			-20
#define ERR_SYNTAX		-21
#define ERR_WRITE		-22
#define ERR_RADIUS		-23
#define ERR_COLOR		-24
#define ERR_NOCAMERA	-25
#define ERR_NOLIGHT		-26
#define ERR_NOOBJ		-27
#define ERR_CYLMEM		-28
#define ERR_NEGSCALE	-29
#define ERR_LABELLEN	-30
#define ERR_VALLEN		-31
#define ERR_LABEL		-32
#define ERR_LABELDBL	-33
#define ERR_NOLABEL		-34

#define PAL_OK			0
#define PAL_FILE		-1
#define PAL_NOTNUM		-2
#define PAL_NUMLEN		-3
#define PAL_LINELEN		-4
#define PAL_NUMNUM		-5
#define PAL_RANGE		-6
#define PAL_LINES		-7

#define ID_NOTHING		0
#define ID_CAMERA		1
#define ID_LIGHT		2
#define ID_SPHERE		3
#define ID_PLANE		4
#define ID_CYLINDER		5

#define CYL_XY			1
#define CYL_XZ			2
#define CYL_YZ			3

#define RES_320X240		1
#define RES_400X300		2
#define RES_640X480		3
#define RES_800X600		4
#define RES_1024X768	5
#define RES_1600X1200	6


//------------------------------------------------------------

#define PUT_VARS() double a, b, c, Delta, sqrt_Delta, t, d1, d2;

#define GET_ROOTS() Delta = b * b - 4.0 * a * c; \
	if (Delta < 0.0) \
		return 0; \
	if (Delta == 0.0) \
		t = -b / 2.0 * a; \
	else { \
		sqrt_Delta = sqrt(Delta); \
		t = 0.5 / a; \
		d1 = (-b - sqrt_Delta) * t; \
		d2 = (-b + sqrt_Delta) * t; \
		if (d1 < d2) \
			t = d1; \
		else \
			t = d2; \
	} \
	if ((t < min_dist) || (t > max_dist)) \
		return 0; \
	*dist = t; \
	return 1;

//------------------------------------------------------------

typedef struct sSCREEN {
	BYTE	*Buffer;
	BYTE	*Palette;
	WORD	Width;
	WORD	Height;
} SCREEN;

typedef struct tVECTOR {
	double x, y, z;
} VECTOR;

typedef struct tSPHERE {
	double		x0, y0, z0;
	double		R;
	BYTE		color;
	struct tSPHERE *prev, *next;
} SPHERE;

typedef struct tPLANE {
	double		x0, y0, z0;
	double		A, B, C;
	BYTE		color1, color2;
	VECTOR		tex_x, tex_y;
	double		tex_scale;
	struct tPLANE *prev, *next;
} PLANE;

typedef struct tCYLINDER {
	double		m, n;
	double		R;
	BYTE		color;
	BYTE		type;
	struct tCYLINDER *prev, *next;
} CYLINDER;

typedef struct tCAMERA {
	VECTOR		pos;
	VECTOR		look_at;
	double		w_div_h;
	double		persp;
	double		fov;
	double		rot;
	SCREEN		scr;
} CAMERA;

typedef struct tLIGHT {
	VECTOR		pos;
} LIGHT;

typedef struct tWORLD {
	CAMERA		cam;
	SPHERE		*sphere_tab;
	int			n_spheres;
	PLANE		*plane_tab;
	int			n_planes;
	CYLINDER	*cylinder_tab;
	int			n_cylinders;
	LIGHT		light;
	char		file[MAX_FILENAME_LEN];
} WORLD;


//------------------------------------------------------------

int LoadPalette(SCREEN *s, char *PalFile);
DWORD SetScreen(SCREEN *s, int w, int h);
void FreeScreen(SCREEN *s);
void ClearScreen(SCREEN *s, BYTE c);
SPHERE *add_sphere(WORLD *w);
PLANE *add_plane(WORLD *w);
CYLINDER *add_cylinder(WORLD *w);
int set_world(WORLD *w, int scr_w, int scr_h, double fov,
	DWORD *allocated_mem);
void free_world(WORLD *w);
int solve_sphere(SPHERE *s, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist);
int solve_plane(PLANE *p, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist);
int solve_cylinder(CYLINDER *cyl, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist);
BYTE shoot_ray(WORLD *w, VECTOR *ro, VECTOR *rd);
void render(WORLD *w, int v);
double vector_len(VECTOR *v);
void cross_product(VECTOR *v1, VECTOR *v2, VECTOR *result);
double dot_product(VECTOR *v1, VECTOR *v2);
int is_double(char *s);
int parse_line(char *line, int *id, int *cyl_type, double *d_tab, BYTE *b_tab);
int set_object(WORLD *w, int id, int type, double *dbl_nums, BYTE *byte_nums);
int build_world(WORLD *w, char *cfgfile, int res,
	int *err_line, int *err_char, DWORD *amem);
DWORD write_to_file(WORLD *w);

#endif
