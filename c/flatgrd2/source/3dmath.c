/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

float sintab[MAX_DEGS],costab[MAX_DEGS];

void init_sincos(void)
{
	int i;
	
	for(i=0;i<MAX_DEGS;i++)
	{
		sintab[i]=sin((float)i*M_PI/(MAX_DEGS/2));
		costab[i]=cos((float)i*M_PI/(MAX_DEGS/2));
	}
}

void translate_point(POINT2D *dest,POINT3D *src)
{
	float temp=src->z+PERSPECTIVE;

	dest->x=((src->x*PERSPECTIVE)/temp)+CORRECT_X;
	dest->y=((src->y*PERSPECTIVE)/temp)+CORRECT_Y;
}

/*
void rotate_point(POINT3D *dest,POINT3D *src,
			short angle_x,short angle_y,short angle_z)
{
	float xr,yr,zr;

	dest->x=src->x;
	dest->y=src->y;
	dest->z=src->z;

	if(angle_x)
	{
		yr=(costab[angle_x]*dest->y)-(sintab[angle_x]*dest->z);
		zr=(sintab[angle_x]*dest->y)+(costab[angle_x]*dest->z);
		dest->y=yr;
		dest->z=zr;
	}

	if(angle_y)
	{
		xr=(costab[angle_y]*dest->x)-(sintab[angle_y]*dest->z);
		zr=(sintab[angle_y]*dest->x)+(costab[angle_y]*dest->z);
		dest->x=xr;
		dest->z=zr;
	}

	if(angle_z)
	{
		xr=(costab[angle_z]*dest->x)-(sintab[angle_z]*dest->y);
		yr=(sintab[angle_z]*dest->x)+(costab[angle_z]*dest->y);
		dest->x=xr;
		dest->y=yr;
	}
}
*/

/* matrix math */

/*
void mx_identity(MATRIX m)
{
	memset((void *)m,0,sizeof(float)*9);
	m[0][0]=m[1][1]=m[2][2]=1;
}

void mx_mul(MATRIX result,MATRIX a,MATRIX b)
{
	result[0][0]=a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];
	result[0][1]=a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];
	result[0][2]=a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2];

	result[1][0]=a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];
	result[1][1]=a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];
	result[1][2]=a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2];

	result[2][0]=a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0];
	result[2][1]=a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1];
	result[2][2]=a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2];
}
*/

void mx_rotation_matrix(MATRIX result,short ax,short ay,short az)
{
	ax&=(MAX_DEGS-1);
	ay&=(MAX_DEGS-1);
	az&=(MAX_DEGS-1);

	result[0][0]=costab[ay]*costab[az];
	result[1][0]=sintab[ax]*sintab[ay]*costab[az]+costab[ax]*(-sintab[az]);
	result[2][0]=costab[ax]*sintab[ay]*costab[az]+(-sintab[ax])*(-sintab[az]);
	result[0][1]=costab[ay]*sintab[az];
	result[1][1]=sintab[ax]*sintab[ay]*sintab[az]+costab[ax]*costab[az];
	result[2][1]=costab[ax]*sintab[ay]*sintab[az]+(-sintab[ax])*costab[az];
	result[0][2]=-sintab[ay];
	result[1][2]=costab[ay]*sintab[ax];
	result[2][2]=costab[ax]*costab[ay];
}

void mx_rotate_points(POINT3D *dest_tab,POINT3D *src_tab,
	int n_points,MATRIX matrix)
{
	int i;
	POINT3D *src,*dest;

	src=src_tab;
	dest=dest_tab;

	for(i=0;i<n_points;i++)
	{
		dest->x=src->x*matrix[0][0]+src->y*matrix[1][0]+src->z*matrix[2][0];
		dest->y=src->x*matrix[0][1]+src->y*matrix[1][1]+src->z*matrix[2][1];
		dest->z=src->x*matrix[0][2]+src->y*matrix[1][2]+src->z*matrix[2][2];	
	
		src++;
		dest++;
	}
}

/* vector math */

void normalize_vector(VECTOR3D *v)
{
	float lenght;

	lenght=sqrt(pow(v->x,2)+pow(v->y,2)+pow(v->z,2));
	if(lenght)
	{
		v->x/=lenght;
		v->y/=lenght;
		v->z/=lenght;
	}
	else
	{
		v->x=0;
		v->y=0;
		v->z=0;
	}
}

void cross_product(VECTOR3D *result,VECTOR3D *v1,VECTOR3D *v2)
{
	result->x=(v1->y*v2->z)-(v1->z*v2->y);
	result->y=(v1->z*v2->x)-(v1->x*v2->z);
	result->z=(v1->x*v2->y)-(v1->y*v2->x);
}

float dot_product(VECTOR3D *v1,VECTOR3D *v2)
{
	return (v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z);
}