/*
	3d Flag/Grid
	Mikolaj Felix 02/08/2001
	mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <math.h>
#include <dos.h>
#include <time.h>
#include <string.h>

#include "graph.c"
#include "3dmath.c"
#include "bresline.c"
#include "triangle.c"
#include "font.c"

#define FLAG_WIDTH 16
#define FLAG_HEIGHT 16
#define FLAG_SIZE FLAG_WIDTH*FLAG_HEIGHT
#define CELL_SIZE 16
#define COS_STEP 20
#define MAX_FACES ((FLAG_WIDTH-1)*(FLAG_HEIGHT-1))*2
#define MAX_LINES MAX_FACES

#define MODE_FLAG 0
#define MODE_GRID 1

#define NO_DELAY 0
#define DELAY 1

typedef struct tFACE
{
	int v1,v2,v3;
}FACE;

typedef struct tLINE
{
	int v1,v2;
}LINE;

POINT3D flag_points[FLAG_SIZE];
POINT3D rotated_flag_points[FLAG_SIZE];
POINT2D translated_flag_points[FLAG_SIZE];
FACE faces[MAX_FACES];
LINE lines[MAX_LINES];

unsigned char colors[FLAG_SIZE];
unsigned angle_x=0,angle_y=0,angle_z=0;

int face_depth[MAX_FACES],order_table[MAX_FACES];

void move_flag(int mode)
{
	static startoff;
	int i,j,cosoff,colortmp=0;
	POINT3D *point;
	unsigned char *colorptr,add_col;

	point=&flag_points[0];
	colorptr=&colors[0];

	for(i=0;i<FLAG_HEIGHT;i++)
		for(j=0;j<FLAG_WIDTH;j++)
		{
			cosoff=((i+j+startoff)*COS_STEP)&(MAX_DEGS-1);
			point->y=cosine[cosoff]*CELL_SIZE;

			if(mode==MODE_FLAG)
			{
				if((++colortmp)&1)
					add_col=0;
				else
					add_col=127;

				*colorptr=(unsigned char)((-cosine[cosoff]*62)+64+1+add_col);
			}
			else
				*colorptr=(unsigned char)((-cosine[cosoff]*63)+64+128);

			point++;
			colorptr++;
		}

	startoff++;
}

void init_lines(void)
{
	int i,j;
	LINE *line;

	line=&lines[0];

	for(i=0;i<(FLAG_HEIGHT-1);i++)
		for(j=0;j<(FLAG_WIDTH-1);j++)
		{
			line->v1=(i*FLAG_WIDTH)+j;
			line->v2=(i*FLAG_WIDTH)+(j+1);
			line++;

			line->v1=(i*FLAG_WIDTH)+j;
			line->v2=((i+1)*FLAG_WIDTH)+j;
			line++;
		}
}

void init_flag(void)
{
	int i,j;
	POINT3D *point;

	point=&flag_points[0];

	for(i=0;i<FLAG_HEIGHT;i++)
		for(j=0;j<FLAG_WIDTH;j++)
		{
			point->x=(FLAG_WIDTH*CELL_SIZE/2)-(i*CELL_SIZE);
			point->z=(FLAG_HEIGHT*CELL_SIZE/2)-(j*CELL_SIZE);

			point++;
		}
}

void rotate_translate_flag(void)
{
	int i;
	POINT3D *srcpoint,*destpoint;
	POINT2D *point2d;

	srcpoint=&flag_points[0];
	destpoint=&rotated_flag_points[0];
	point2d=&translated_flag_points[0];

	for(i=0;i<FLAG_SIZE;i++)
	{
		rotate_point(destpoint,srcpoint,angle_x,angle_y,angle_z);
		translate_point(point2d,destpoint);
		
		srcpoint++;
		destpoint++;
		point2d++;
	}
}

void depth_sorting(int left,int right)
{
	int i,j,tmp,element;

	i=left;
	j=right;
	element=face_depth[(left+right)>>1];

	while(i<j)
	{
		while(face_depth[i]<element) i++;
		while(face_depth[j]>element) j--;

		if(i<=j)
		{
			tmp=face_depth[i];
			face_depth[i]=face_depth[j];
			face_depth[j]=tmp;

			tmp=order_table[i];
			order_table[i]=order_table[j];
			order_table[j]=tmp;

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

	face=&faces[0];

	for(i=0;i<MAX_FACES;i++)
	{
		order_table[i]=i;

		vertex=&rotated_flag_points[face->v1];
		depth=vertex->z;
		vertex=&rotated_flag_points[face->v2];
		depth+=vertex->z;
		vertex=&rotated_flag_points[face->v3];
		depth+=vertex->z;

		face_depth[i]=(int)(depth*64);

		face++;
	}

	depth_sorting(0,MAX_FACES-1);
}

void draw_flag(int mode)
{
	int i,x1,y1,x2,y2,x3,y3;
	unsigned char color;
	FACE *face;
	LINE *line;
	POINT2D *point;

	if(mode==MODE_FLAG)
	{
		sort_faces();

		for(i=0;i<MAX_FACES;i++)
		{
			face=&faces[order_table[MAX_FACES-1-i]];

			point=&translated_flag_points[face->v1];
			x1=point->x;
			y1=point->y;
			point=&translated_flag_points[face->v2];
			x2=point->x;
			y2=point->y;
			point=&translated_flag_points[face->v3];
			x3=point->x;
			y3=point->y;

			color=colors[face->v1];
			triangle(x1,y1,x2,y2,x3,y3,color,frame_buffer);
		}
	}
	else
	{
		line=&lines[0];

		for(i=0;i<MAX_LINES;i++)
		{
			point=&translated_flag_points[line->v1];
			x1=point->x;
			y1=point->y;
			point=&translated_flag_points[line->v2];
			x2=point->x;
			y2=point->y;

			color=colors[line->v1];
			bresline(x1,y1,x2,y2,color,frame_buffer);

			line++;
		}
	}
}

void init_palette(int mode)
{
	int i;

	if(mode==MODE_FLAG)
	{
		outp(0x03c8,0);
		outp(0x03c9,0);
		outp(0x03c9,0);
		outp(0x03c9,0);

		for(i=0;i<127;i++)
		{
			outp(0x03c9,(i/3)+15);
			outp(0x03c9,0);
			outp(0x03c9,0);
		}
		for(i=0;i<127;i++)
		{
			outp(0x03c9,(i/3)+15);
			outp(0x03c9,(i/3)+15);
			outp(0x03c9,(i/3)+15);
		}
	}
	else
	{
		outp(0x03c8,128);
		for(i=0;i<128;i++)
		{
			outp(0x03c9,(i/3)+16);
			outp(0x03c9,(i/3)+16);
			outp(0x03c9,(i/3)+16);
		}
	}
}

void init_faces(void)
{
	int i,j;
	FACE *face;

	face=&faces[0];

	for(i=0;i<(FLAG_HEIGHT-1);i++)
		for(j=0;j<(FLAG_WIDTH-1);j++)
		{
			face->v1=(i*FLAG_WIDTH)+j;
			face->v2=(i*FLAG_WIDTH)+(j+1);
			face->v3=((i+1)*FLAG_WIDTH)+(j+1);
			face++;

			face->v1=(i*FLAG_WIDTH)+j;
			face->v2=((i+1)*FLAG_WIDTH)+(j+1);
			face->v3=((i+1)*FLAG_WIDTH)+j;
			face++;
		}
}

void main(int argc,char **argv)
{
	time_t before,after;
	unsigned long frames;
	float framerate;
	char txt_buffer[32];
	int mode=MODE_FLAG,display_mode;

	if(argc<2)
		display_mode=DELAY;
	else
	{
		if(strncmp(argv[1],"-nd",3)==0)
			display_mode=NO_DELAY;
		else
		{
			printf("%s","Incorrect argument!\n");
			exit(1);
		}
	}

	if((frame_buffer=(unsigned char *)malloc(BUFFER_SIZE))==NULL)
		exit(2);

	init_sincos();
	init_font();
	init_flag();
	init_faces();
	init_lines();

	set_vga();
	init_palette(mode);

	before=time(NULL);
	frames=0;

	while(mode<2)
	{
		move_flag(mode);

		rotate_translate_flag();
		draw_flag(mode);

		put_string("3d Flag/Grid",0,0,240,frame_buffer);
		put_string("Mikolaj Felix 02/08/01",0,12,240,frame_buffer);
		put_string("mfelix@polbox.com",0,24,240,frame_buffer);

		frames++;
		after=time(NULL);

		if((after-before))
		{
			framerate=(float)(frames)/(float)(after-before);
			gcvt(framerate,5,txt_buffer);
		}
		else
			txt_buffer[0]=0;

		if(display_mode==DELAY)
		{
			retrace();
			retrace();
		}
		else
		{
			put_string("FPS: ",0,36,240,frame_buffer);
			put_string(txt_buffer,40,36,240,frame_buffer);
		}

		copy_buffer();
		clear_buffer();

		angle_x++;
		angle_x&=(MAX_DEGS-1);
		angle_y++;
		angle_y&=(MAX_DEGS-1);
		angle_z++;
		angle_z&=(MAX_DEGS-1);

		if(kbhit())
		{
			getch();
			mode++;
			init_palette(mode);
			frames=0;
			before=time(NULL);
		}
	}

	set_text();
	free(frame_buffer);
}