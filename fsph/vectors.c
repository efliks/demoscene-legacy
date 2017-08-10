#define VECTOR POINT3D

void make_unitvector(VECTOR *v,
					float x1,float y1,float z1,float x2,float y2,float z2)
{
	float lenght;
	float x,y,z;

	x=x2-x1;
	y=y2-y1;
	z=z2-z1;

	lenght=sqrt(x*x+y*y+z*z);

	if(lenght)
	{
		v->x=x/lenght;
		v->y=y/lenght;
		v->z=z/lenght;
	}
	else
	{
		v->x=0;
		v->y=0;
		v->z=0;
	}
}