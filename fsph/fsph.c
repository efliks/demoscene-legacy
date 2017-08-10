/*
	Filled Sphere
	Mikolaj Felix 01/08/2001
	mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <math.h>
#include <time.h>
#include <dos.h>
#include <string.h>

#include "fsph\\graph.c"
#include "fsph\\3dmath.c"
#include "fsph\\triangle.c"
#include "fsph\\font.c"
#include "fsph\\vectors.c"

#define POINTS_PER_CIRCLE 20
#define SPHERE_RADIUS 90
#define MAX_POINTS POINTS_PER_CIRCLE*POINTS_PER_CIRCLE
#define ANGLE_STEP (float)(M_PI/(180/POINTS_PER_CIRCLE))
#define MAX_FACES ((POINTS_PER_CIRCLE-1)*(POINTS_PER_CIRCLE-1))*2

#define TEXT_COLOR 64

typedef struct tFACE
{
	int v1,v2,v3;
}FACE;

POINT3D sphere[MAX_POINTS];
POINT3D rotated_sphere[MAX_POINTS];
POINT2D translated_sphere[MAX_POINTS];
FACE faces[MAX_FACES];
VECTOR face_normals[MAX_FACES],light_source;

int visible_triangles;
unsigned angle_x=0,angle_y=0,angle_z=0;

void init_sphere(void)
{
	int i,j;
	POINT3D *point;
	float sphere_angle,circle_angle;
	FACE *face;

	point=&sphere[0];
	sphere_angle=0;

	for(i=0;i<POINTS_PER_CIRCLE;i++)
	{
		circle_angle=0;

		for(j=0;j<POINTS_PER_CIRCLE;j++)
		{
			point->x=sin(circle_angle)*SPHERE_RADIUS;
			point->y=cos(circle_angle)*SPHERE_RADIUS;
			point->z=sin(sphere_angle)*point->x;
			point->x*=cos(sphere_angle);

			point++;
			circle_angle+=ANGLE_STEP;
		}

		sphere_angle+=ANGLE_STEP;
	}
}

void init_faces(void)
{
	int i,j;
	FACE *face;

	face=&faces[0];

	for(j=(POINTS_PER_CIRCLE-1);j>0;j--)
		for(i=0;i<(POINTS_PER_CIRCLE-1);i++)
		{
			face->v1=(i*POINTS_PER_CIRCLE)+j;
			face->v2=(i*POINTS_PER_CIRCLE)+(j-1);
			face->v3=((i+1)*POINTS_PER_CIRCLE)+(j-1);
			face++;

			face->v1=(i*POINTS_PER_CIRCLE)+j;
			face->v2=((i+1)*POINTS_PER_CIRCLE)+(j-1);
			face->v3=((i+1)*POINTS_PER_CIRCLE)+j;
			face++;
		}
}

void draw_sphere(void)
{
	int i,x1,y1,x2,y2,x3,y3;
	FACE *face;
	POINT2D *point;
	VECTOR rotnrm,*rotated_normal,*light;
	unsigned char color;
	float dot_product;

	face=&faces[0];
	rotated_normal=&rotnrm;
	light=&light_source;
	visible_triangles=0;

	for(i=0;i<MAX_FACES;i++)
	{
		point=&translated_sphere[face->v1];
		x1=point->x;
		y1=point->y;
		point=&translated_sphere[face->v2];
		x2=point->x;
		y2=point->y;
		point=&translated_sphere[face->v3];
		x3=point->x;
		y3=point->y;

		if(((y1-y3)*(x2-x1)-(x1-x3)*(y2-y1))>0)
		{
			rotate_point(rotated_normal,&face_normals[i],angle_x,angle_y,angle_z);

			dot_product=(rotated_normal->x*light->x)+
						(rotated_normal->y*light->y)+
						(rotated_normal->z*light->z);
						
			if(dot_product>1 || dot_product<0) dot_product=0;
			color=(unsigned char)(dot_product*63);

			triangle(x1,y1,x2,y2,x3,y3,color,frame_buffer);
			visible_triangles++;
		}

		face++;
	}
}

void rotate_translate_sphere(void)
{
	int i;

	for(i=0;i<MAX_POINTS;i++)
	{
		rotate_point(&rotated_sphere[i],&sphere[i],angle_x,angle_y,angle_z);
		translate_point(&translated_sphere[i],&rotated_sphere[i]);
	}
}

void init_palette(void)
{
	int i;
	unsigned char b;

	outp(0x03c8,0);
	for(i=0;i<64;i++)
	{
		outp(0x03c9,i);
		outp(0x03c9,i);
		outp(0x03c9,0);
	}
	for(i=0;i<8;i++)
	{
		outp(0x03c9,(b=(i*3)+30));
		outp(0x03c9,b);
		outp(0x03c9,b);
	}
}

void init_face_normals(void)
{
	int i;
	FACE *face;
	VECTOR *normal;
	float x_normal,y_normal,z_normal;
	POINT3D *p1,*p2,*p3;

	face=&faces[0];
	normal=&face_normals[0];

	for(i=0;i<MAX_FACES;i++)
	{
		p1=&sphere[face->v1];
		p2=&sphere[face->v2];
		p3=&sphere[face->v3];

		x_normal=(p2->y-p1->y)*(p1->z-p3->z)-(p2->z-p1->z)*(p1->y-p3->y);
		y_normal=(p2->z-p1->z)*(p1->x-p3->x)-(p2->x-p1->x)*(p1->z-p3->z);
		z_normal=(p2->x-p1->x)*(p1->y-p3->y)-(p2->y-p1->y)*(p1->x-p3->x);		

		make_unitvector(normal,0,0,0,x_normal,y_normal,z_normal);

		face++;
		normal++;
	}
}

void main(void)
{
	char txt_buffer1[64],txt_buffer2[64];
	time_t before,after;
	unsigned long frames;
	float framerate;

	if((frame_buffer=(unsigned char *)malloc(BUFFER_SIZE))==NULL)
		exit(1);

	randomize();

	init_font();
	init_sincos();
	init_sphere();
	init_faces();
	init_face_normals();

	/* init light */

	make_unitvector(&light_source,0,0,-100,0,0,0);

	set_vga();
	init_palette();

	before=time(NULL);
	frames=0;

	while(!kbhit())
	{
		rotate_translate_sphere();
		draw_sphere();

		itoa(visible_triangles,txt_buffer1,10);
		strcpy(txt_buffer2,txt_buffer1);
		strcat(txt_buffer2,"/");

		frames++;
		after=time(NULL);

		itoa((after-before),txt_buffer1,10);
		strcat(txt_buffer2,txt_buffer1);
		strcat(txt_buffer2,"/");

		ltoa(frames,txt_buffer1,10);
		strcat(txt_buffer2,txt_buffer1);
		strcat(txt_buffer2,"/");

		if((after-before))
		{
			framerate=(float)(frames)/(float)(after-before);
			gcvt(framerate,5,txt_buffer1);
			strcat(txt_buffer2,txt_buffer1);
		}

		put_string(txt_buffer2,0,1,TEXT_COLOR,frame_buffer);

		retrace();
		copy_buffer();
		clear_buffer();

		angle_x++;
		angle_x&=(MAX_DEGS-1);
		angle_y+=3;
		angle_y&=(MAX_DEGS-1);
		angle_z++;
		angle_z&=(MAX_DEGS-1);
	}

	set_text();
	free(frame_buffer);
}