#define MAX_DEGS 512

#define PERSPECTIVE 200.0
#define CORRECT_X 160.0
#define CORRECT_Y 100.0

typedef struct tPOINT3D
{
	float x,y,z;
}POINT3D;

typedef struct tPOINT2D
{
	int x,y;
}POINT2D;

float sine[MAX_DEGS],cosine[MAX_DEGS];

void init_sincos(void)
{
	int i;
	
	for(i=0;i<MAX_DEGS;i++)
	{
		sine[i]=sin((float)i*M_PI/(MAX_DEGS/2));
		cosine[i]=cos((float)i*M_PI/(MAX_DEGS/2));
	}
}

void translate_point(POINT2D *dest,POINT3D *src)
{
	float temp=src->z+PERSPECTIVE;

	dest->x=((src->x*PERSPECTIVE)/temp)+CORRECT_X;
	dest->y=((src->y*PERSPECTIVE)/temp)+CORRECT_Y;
}

void rotate_point(POINT3D *dest,POINT3D *src,
			unsigned angle_x,unsigned angle_y,unsigned angle_z)
{
	float xr,yr,zr;

	dest->x=src->x;
	dest->y=src->y;
	dest->z=src->z;

	if(angle_x)
	{
		yr=(cosine[angle_x]*dest->y)-(sine[angle_x]*dest->z);
		zr=(sine[angle_x]*dest->y)+(cosine[angle_x]*dest->z);
		dest->y=yr;
		dest->z=zr;
	}

	if(angle_y)
	{
		xr=(cosine[angle_y]*dest->x)-(sine[angle_y]*dest->z);
		zr=(sine[angle_y]*dest->x)+(cosine[angle_y]*dest->z);
		dest->x=xr;
		dest->z=zr;
	}

	if(angle_z)
	{
		xr=(cosine[angle_z]*dest->x)-(sine[angle_z]*dest->y);
		yr=(sine[angle_z]*dest->x)+(cosine[angle_z]*dest->y);
		dest->x=xr;
		dest->y=yr;
	}
}