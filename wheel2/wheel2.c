/*
	Mikolaj Felix 16/09/01, 30/09/2001
	mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

#include "hicolor\\sys.c"
#include "hicolor\\rndpal.c"
#include "hicolor\\grd3a.c"
#include "hicolor\\bmp.c"
#include "hicolor\\scrshot.c"
#include "hicolor\\font.c"

/*
#include "libs\\3dmath.c"
#include "libs\\matrix.c"
#include "libs\\vectors.c"
*/

#include "libs\\3dmath.h"
#include "libs\\3dmath.c"

#include "torus.h" //"wheel3d.h"
#include "title.h"

#define MAX_PHONG_COLORS 63

POINT3D r_points[MAX_P];
POINT2D t_points[MAX_P];
VECTOR3D p_normals[MAX_P],rp_normals[MAX_P];
VECTOR3D light;
short face_palettes[MAX_F][MAX_PHONG_COLORS+1];

int face_depths[MAX_F],face_order[MAX_F];
int visible_faces;

unsigned char envmap[256*256];

void init_envmap(void)
{
	float x,y,temp_color;
	unsigned char *ptr,color;

	ptr=envmap;

	for(y=-128;y<128;y++)
		for(x=-128;x<128;x++)
		{
			temp_color=127-sqrt(pow(x,2)+pow(y,2));

			if(temp_color<0)
				temp_color=0;
			else if(temp_color>127)
				temp_color=127;

			color=(unsigned char)temp_color>>1;
			*ptr=color;
			ptr++;
		}
}

void init_face_palettes(void)
{
	int i,j;

	srand((int)rawclock());

	for(i=0;i<MAX_F;i+=2)
	{
		random_phong_palette(face_palettes[i],MAX_PHONG_COLORS);
		memcpy(face_palettes[i+1],face_palettes[i],(MAX_PHONG_COLORS+1)*2);
	}
}

void init_normals(void)
{
	int i,j,face_hit;
	//float x_normal,y_normal,z_normal;
	float sum_x,sum_y,sum_z;
	VECTOR3D face_normal,v1,v2;
	POINT3D *p1,*p2,*p3;

	for(i=0;i<MAX_P;i++)
	{
		sum_x=sum_y=sum_z=0;
		face_hit=0;

		for(j=0;j<MAX_F;j++)
		{
			if(faces[j].v1==i || faces[j].v2==i || faces[j].v3==i)
			{
				p1=&points[faces[j].v1];
				p2=&points[faces[j].v2];
				p3=&points[faces[j].v3];

				v1.x=p2->x-p1->x;
				v1.y=p2->y-p1->y;
				v1.z=p2->z-p1->z;

				v2.x=p3->x-p2->x;
				v2.y=p3->y-p2->y;
				v2.z=p3->z-p2->z;

				cross_product(&face_normal,&v1,&v2);

				/*
				x_normal=(p2->y-p1->y)*(p1->z-p3->z)-(p2->z-p1->z)*(p1->y-p3->y);
				y_normal=(p2->z-p1->z)*(p1->x-p3->x)-(p2->x-p1->x)*(p1->z-p3->z);
				z_normal=(p2->x-p1->x)*(p1->y-p3->y)-(p2->y-p1->y)*(p1->x-p3->x);		

				make_unitvector(&face_normal,0,0,0,x_normal,y_normal,z_normal);
				*/

				sum_x+=face_normal.x;
				sum_y+=face_normal.y;
				sum_z+=face_normal.z;

				face_hit++;
			}
		}

		sum_x/=face_hit;
		sum_y/=face_hit;
		sum_z/=face_hit;

		v1.x=sum_x;
		v1.y=sum_y;
		v1.z=sum_z;

		normalize_vector(&v1);

		p_normals[i].x=v1.x;
		p_normals[i].y=v1.y;
		p_normals[i].z=v1.z;

		//make_unitvector(&p_normals[i],0,0,0,sum_x,sum_y,sum_z);
	}
}

void rotate_translate_object(void)
{
	int i;
	static unsigned angle_x,angle_y,angle_z;
	MATRIX rot_matrix;

	mx_rotation_matrix(rot_matrix,angle_x,angle_y,angle_z);

	mx_rotate_points(r_points,points,MAX_P,rot_matrix);
	mx_rotate_points(rp_normals,p_normals,MAX_P,rot_matrix);

	for(i=0;i<MAX_P;i++)
		translate_point(&t_points[i],&r_points[i]);

	/*
	for(i=0;i<MAX_P;i++)
	{
		rotate_point(&r_points[i],&points[i],ax,ay,az);
		translate_point(&t_points[i],&r_points[i]);

		rotate_point(&rp_normals[i],&p_normals[i],ax,ay,az);
	}
	*/

	angle_x++;
	angle_y+=3;
	angle_z++;
}

void depth_sorting(int left,int right)
{
	int i,j,tmp,element;

	i=left;
	j=right;
	element=face_depths[(left+right)>>1];

	while(i<j)
	{
		while(face_depths[i]<element) i++;
		while(face_depths[j]>element) j--;

		if(i<=j)
		{
			tmp=face_depths[i];
			face_depths[i]=face_depths[j];
			face_depths[j]=tmp;

			tmp=face_order[i];
			face_order[i]=face_order[j];
			face_order[j]=tmp;

			i++;
			j--;
		}

	}

	if(j>left) depth_sorting(left,j);
	if(i<right) depth_sorting(i,right);
}

void sort_faces(void)
{
	int i,j;
	FACE *face;
	POINT3D *vertex;
	float depth;
	int x1,y1,x2,y2,x3,y3;
	POINT2D *v2d;

	face=faces;
	visible_faces=0;

	for(i=0;i<MAX_F;i++,face++)
	{
		v2d=&t_points[face->v1];
		x1=v2d->x;
		y1=v2d->y;
		v2d=&t_points[face->v2];
		x2=v2d->x;
		y2=v2d->y;
		v2d=&t_points[face->v3];
		x3=v2d->x;
		y3=v2d->y;

		if(((y1-y3)*(x2-x1)-(x1-x3)*(y2-y1))<=0)
			continue;

		face_order[visible_faces]=i;

		vertex=&r_points[face->v1];
		depth=vertex->z;
		vertex=&r_points[face->v2];
		depth+=vertex->z;
		vertex=&r_points[face->v3];
		depth+=vertex->z;

		face_depths[visible_faces]=(int)(depth*64);
		visible_faces++;
	}

	depth_sorting(0,visible_faces-1);
}

void draw_faces(void)
{
	int i,face_nr;
	int x1,y1,x2,y2,x3,y3;

	float tc1,tc2,tc3;
	int col1,col2,col3;

	int tx1,ty1,tx2,ty2,tx3,ty3;

	for(i=0;i<visible_faces;i++)
	{
		face_nr=face_order[visible_faces-i-1];

		x1=t_points[faces[face_nr].v1].x;
		y1=t_points[faces[face_nr].v1].y;
		x2=t_points[faces[face_nr].v2].x;
		y2=t_points[faces[face_nr].v2].y;
		x3=t_points[faces[face_nr].v3].x;
		y3=t_points[faces[face_nr].v3].y;

		tc1=dot_product(&rp_normals[faces[face_nr].v1],&light);
		tc2=dot_product(&rp_normals[faces[face_nr].v2],&light);
		tc3=dot_product(&rp_normals[faces[face_nr].v3],&light);

		/*
		tc1=rp_normals[faces[face_nr].v1].x*light.x+
			rp_normals[faces[face_nr].v1].y*light.y+
			rp_normals[faces[face_nr].v1].z*light.z;

		tc2=rp_normals[faces[face_nr].v2].x*light.x+
			rp_normals[faces[face_nr].v2].y*light.y+
			rp_normals[faces[face_nr].v2].z*light.z;

		tc3=rp_normals[faces[face_nr].v3].x*light.x+
			rp_normals[faces[face_nr].v3].y*light.y+
			rp_normals[faces[face_nr].v3].z*light.z;
		*/

		if(tc1<0 || tc1>1) tc1=0;
		if(tc2<0 || tc2>1) tc2=0;
		if(tc3<0 || tc3>1) tc3=0;

		col1=tc1*MAX_PHONG_COLORS;
		col2=tc2*MAX_PHONG_COLORS;
		col3=tc3*MAX_PHONG_COLORS;

		gouraud_triangle(x1,y1,x2,y2,x3,y3,
			col1,col2,col3,face_palettes[face_nr]);
	}
}

void put_transparent_bmp(unsigned char *data,
							int width,int height,int x,int y)
{
	/*
		data	ebp+8
		width	ebp+12
		height	ebp+16
		x		ebp+20
		y		ebp+24
	*/

	asm volatile("movl 24(%%ebp),%%eax;
		movl %%eax,%%ebx;
		shll $6,%%eax;
		shll $8,%%ebx;
		addl %%ebx,%%eax;
		addl 20(%%ebp),%%eax;
		shll $1,%%eax;
		addl %%eax,%%edi;

		movl $320,%%edx;
		subl 12(%%ebp),%%edx;
		shll $1,%%edx;				/* edx - add_off */

		cld;

		movl 16(%%ebp),%%ecx;
ver:
		movl 12(%%ebp),%%ebx;
hor:
		lodsw;
		orw %%ax,%%ax;
		jz next;

		movw %%ax,(%%edi);
next:
		addl $2,%%edi;
		decl %%ebx;
		jnz hor;

		addl %%edx,%%edi;
		decl %%ecx;
		jnz ver"
		:
		: "S" (data), "D" (frame_buffer)
		: "%eax", "%ebx", "%ecx", "%edx", "cc", "memory");

	/*
	unsigned char *ofs,*src;
	int i,j,add_ofs;
	unsigned color1,color2;
	unsigned char b1,b2;

	ofs=frame_buffer;
	ofs+=(((y<<6)+(y<<8)+x)<<1);
	src=data;
	add_ofs=(320-width)<<1;

	for(i=0;i<height;i++,ofs+=add_ofs)
		for(j=0;j<width;j++)
		{
			if(*src)
			{
				*ofs=*src;
				*(ofs+1)=*(src+1);		
			}
			ofs+=2;
			src+=2;
		}
	*/
}

int main(void)
{
	//BMP *title;
	char key;
	int text_active=0;

	/*title=load_bmp("title.bmp");
	if(!title)
	{
		printf("%s%s",bmp_message,"\n");
		return 1;
	}*/

	if(!enable_graph())
	{
		//unload_bmp(title);
		return 2;
	}

	init_font();
	init_sincos();
	init_face_palettes();
	init_normals();
	
	//make_unitvector(&light,0,0,-100,0,0,0);

	light.x=0;
	light.y=0;
	light.z=-100;
	normalize_vector(&light);

	while(1)
	{
		if(kbhit())
		{
			if((key=getch())==27)
				break;
			else if(key==13 && !text_active)
			{
				screenshot();
				text_active=150;
			}
		}

		clear_buffer();

		rotate_translate_object();
		sort_faces();
		draw_faces();

		put_transparent_bmp(title,TITLE_WIDTH,TITLE_HEIGHT,0,0);

		if(text_active)
		{
			put_string("Screen saved.",0,191,COLOR16(31,31,31));
			text_active--;
		}

		retrace();
		copy_buffer();
	}

	disable_graph();

	//unload_bmp(title);

	printf("%s","Code by Mikolaj Felix (mfelix@polbox.com)\n");
	printf("%s%s%s%s%s","Build date/time: ",__DATE__," ",__TIME__,"\n");
	return 0;
}