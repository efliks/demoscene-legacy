/*
	Flat Polygons with Shadow
	Gouraud Shading (Moving Light)

	Mikolaj Felix 20/06/2001
	mfelix@polbox.com

	Texture Mapping & PCX support added on 13/07/2001
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "graph.c"
#include "3dmath.c"
#include "pcx.c"

#define MAX_POINTS 8
#define MAX_FACES 6
#define SIZE 50

#define LEFT 0
#define RIGHT 1

#define SHADOW_X -40
#define SHADOW_Y 40

#define xchg(a,b) { asm { mov ax,a; xchg ax,b; mov a,ax } }
#define xchg8(a,b) { asm { mov al,a; xchg b,al; mov a,al } }

#define FILLED 0
#define GOURAUD 1
#define TEXTURED 2

typedef struct tPOLYGON
{
	int v1,v2,v3,v4;
	unsigned char color;
}POLYGON;

POINT3D object[MAX_POINTS]={{-SIZE,-SIZE,SIZE},{SIZE,-SIZE,SIZE},{SIZE,SIZE,SIZE},{-SIZE,SIZE,SIZE},
			{-SIZE,-SIZE,-SIZE},{SIZE,-SIZE,-SIZE},{SIZE,SIZE,-SIZE},{-SIZE,SIZE,-SIZE}};

POLYGON polygons[MAX_FACES]={{0,1,2,3,45},{5,4,7,6,40},{1,5,6,2,80},{4,0,3,7,14},
				{4,5,1,0,1},{3,2,6,7,32}};

POINT3D light,light_rotated;
POINT3D normal[MAX_FACES],normal_rotated[MAX_FACES];
POINT3D point_normal[MAX_POINTS],point_normal_rotated[MAX_POINTS];

POINT3D object_rotated[MAX_POINTS];
POINT2D object_translated[MAX_POINTS];

unsigned char *frame_buffer,*background;

int polygon_edges[200][2];
unsigned char polygon_colors[200][2];

#include "texpol.c"

unsigned char pol_texture[128*128];

void force_black_color_at_0(PCX *pcx)
{
	unsigned i,new_black_color;
	unsigned char b1,b2;

	for(i=0;i<768;i+=3)
		if(!palette[i] && !palette[i+1] && !palette[i+2]) break;
	
	new_black_color=i;
	
	for(i=0;i<3;i++)
	{
		b1=palette[i];
		b2=palette[new_black_color+i];
		palette[i]=b2;
		palette[new_black_color+i]=b1;
	}

	for(i=0;i<pcx->width*pcx->height;i++)
		if(!pcx->data[i]) pcx->data[i]=new_black_color;
}

void generate_pol_texture(void)
{
	int x,y,off=0;

	for(y=0;y<128;y++)
			for(x=0;x<128;x++) pol_texture[off++]=((x^y)&63)+128+64;
}

void init_pol_texture(void)
{
	PCX pcx;

	if(load_pcx(&pcx,"texture.pcx",1))
	{
		if(pcx.width!=128 || pcx.height!=128)
		{
			unload_pcx(&pcx);
			generate_pol_texture();
		}
		else
		{
			memcpy(pol_texture,pcx.data,128*128);
			force_black_color_at_0(&pcx);
			unload_pcx(&pcx);
			set_palette();
		}
	}
	else
		generate_pol_texture();
}

void make_light_unitvector(float x1,float y1,float z1,
						float x2,float y2,float z2)
{
	float lenght,x,y,z;

	x=x2-x1;
	y=y2-y1;
	z=z2-z1;

	lenght=sqrt(x*x+y*y+z*z);

	light.x=x/lenght;
	light.y=y/lenght;
	light.z=z/lenght;
}

void set_palette_light(void)
{
	int i;
	unsigned char b;

	outp(0x03c8,0);

	outp(0x03c9,0);
	outp(0x03c9,0);
	outp(0x03c9,0);

	for(i=1;i<64-20;i++)
	{
		outp(0x03c9,20+i);
		outp(0x03c9,20+i);
		outp(0x03c9,0);
	}

	for(i=0;i<20;i++)
	{
		outp(0x03c9,63);
		outp(0x03c9,63);
		outp(0x03c9,63-20+i);
	}
}

void set_palette2(void)
{
	int i;

	outp(0x03c8,128);
	for(i=0;i<128;i++)
	{
		outp(0x03c9,i>>1);
		outp(0x03c9,i>>1);
		outp(0x03c9,i>>1);
	}
}

void draw_object_points(void)
{
	int i,x,y;

	for(i=0;i<MAX_POINTS;i++)
	{
		x=object_translated[i].x;
		y=object_translated[i].y;

		if(x>=0 && x<=319 && y>=0 && y<=199)
			frame_buffer[(y<<6)+(y<<8)+x]=31;
	}
}

void horizontal_line(int x1,int x2,int y,unsigned char color)
{
	asm {
		mov ax,x1
		cmp ax,x2
		je quit
		jb ok
		xchg ax,x2
		mov x1,ax }
ok:
	asm {
		push es
		les di,frame_buffer
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x1
		add di,ax

		mov al,color
		mov ah,al

		mov cx,x2
		sub cx,x1
		inc cx
		shr cx,1
		jnc draw
		stosb }
draw:
	asm {
		rep stosw
		pop es }
quit:	
}

void gouraud_line(int x1,int x2,int y,
					unsigned char color1,unsigned char color2)
{
	asm {
		mov ax,x1
		cmp ax,x2
		je quit
		jb ok

		xchg ax,x2
		mov x1,ax

		mov al,color1
		xchg al,color2
		mov color1,al }
ok:
	asm {
		push es
		les di,frame_buffer
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x1
		add di,ax

		xor ah,ah
		mov al,color2
		xor bh,bh
		mov bl,color1
		sub ax,bx
		shl ax,7
		cwd
		mov cx,x2
		sub cx,x1
		idiv cx
		mov dx,ax

		xor bh,bh
		mov bl,color1
		shl bx,7
		inc cx
		cld }
draw:
	asm {
		mov ax,bx
		shr ax,7
		stosb
		add bx,dx
		dec cx
		jnz draw
		pop es }
quit:
}

void horizontal_shadow_smooth_line(int x1,int x2,int y,
					unsigned char c1,unsigned char c2)
{
	int i,off1,off2,delta_c,cur_c;
	unsigned char b;

	if((y+SHADOW_Y)<0 || (y+SHADOW_Y)>199)
	{
		gouraud_line(x1,x2,y,c1,c2);
		return;
	}

	if(x1>x2)
		xchg(x1,x2);
	if(x1==x2)
		return;

	delta_c=((c2-c1)<<7)/(x2-x1);
	cur_c=c1<<7;

	off1=(y<<6)+(y<<8)+x1;
	off2=((y+SHADOW_Y)<<6)+((y+SHADOW_Y)<<8);

	for(i=x1;i<=x2;i++)
	{
		if((i+SHADOW_X)>0 && (i+SHADOW_X)<320)
		{
			b=frame_buffer[off2+i+SHADOW_X];

			if(b>127)
			{
				b&=63;
				frame_buffer[off2+i+SHADOW_X]=b+128;
			}
		}

		frame_buffer[off1++]=cur_c>>7;
		cur_c+=delta_c;
	}
}

void horizontal_shadow_line(int x1,int x2,int y,unsigned char color)
{
	asm {
		mov ax,y
		add ax,SHADOW_Y
		or ax,ax
		jl exec_line
		cmp ax,199
		jg exec_line

		mov ax,x1
		cmp ax,x2
		je quit
		jb ok

		xchg ax,x2
		mov x1,ax }
ok:
	asm {
		push es
		les di,frame_buffer
		mov si,di
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x1
		add di,ax

		mov ax,y
		add ax,SHADOW_Y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,SHADOW_X
		add si,ax
		mov bx,x1
		add si,bx

		mov al,color
		mov cx,x2
		sub cx,x1
		inc cx }
draw:
	asm {
		or bx,bx
		jl next
		cmp bx,319
		jg next

		mov dl,byte ptr es:[si]
		cmp dl,127
		jbe next

		and dl,63
		add dl,128
		mov byte ptr es:[si],dl }
next:
	asm {
		stosb
		inc bx
		inc si
		dec cx
		jnz draw
		pop es
		jmp quit }

exec_line:
		horizontal_line(x1,x2,y,color);
quit:
}

/*
void horizontal_shadow_line(int x1,int x2,int y,unsigned char color)
{
	int i,off1,off2;
	unsigned char b;

	if((y+SHADOW_Y)<0 || (y+SHADOW_Y)>199)
	{
		horizontal_line(x1,x2,y,color);
		return;
	}

	if(x1>x2) xchg(x1,x2);

	off1=(y<<6)+(y<<8)+x1;
	off2=((y+SHADOW_Y)<<6)+((y+SHADOW_Y)<<8);

	for(i=x1;i<=x2;i++)
	{
		if((i+SHADOW_X)>0 && (i+SHADOW_X)<320)
		{
			b=frame_buffer[off2+i+SHADOW_X];

			if(b>127)
			{
				b&=63;
				frame_buffer[off2+i+SHADOW_X]=b+128;
			}
		}

		frame_buffer[++off1]=color;
	}
}
*/

void gouraud_scan_edge(int x1,int y1,int x2,int y2,
						unsigned char color1,unsigned char color2)
{
	int i,deltax,x,xfixed;
	int delta_c,fixed_c,color;

	if(y1==y2) return;

	if(y1>y2)
	{
		xchg(y1,y2);
		xchg(x1,x2);
		xchg8(color1,color2);
	}

	deltax=((x2-x1)<<7)/(y2-y1);
	xfixed=x1<<7;

	delta_c=((color2-color1)<<7)/(y2-y1);
	fixed_c=color1<<7;

	for(i=y1;i<y2;i++)
	{
		x=xfixed>>7;
		color=fixed_c>>7;

		if(x<polygon_edges[i][LEFT])
		{
			polygon_edges[i][LEFT]=x;
			polygon_colors[i][LEFT]=color;
		}		

		if(x>polygon_edges[i][RIGHT])
		{
			polygon_edges[i][RIGHT]=x;
			polygon_colors[i][RIGHT]=color;		
		}

		xfixed+=deltax;
		fixed_c+=delta_c;
	}
}

void gouraud_polygon(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,
					unsigned char c1,unsigned char c2,unsigned char c3,unsigned char c4)
{
	int min_y,max_y,i;

	for(i=0;i<200;i++)
	{
		polygon_edges[i][LEFT]=32000;
		polygon_edges[i][RIGHT]=-32000;
	}

	min_y=y1;
	if(y2<min_y) min_y=y2;
	if(y3<min_y) min_y=y3;
	if(y4<min_y) min_y=y4;

	max_y=y1;
	if(y2>max_y) max_y=y2;
	if(y3>max_y) max_y=y3;
	if(y4>max_y) max_y=y4;

	if(min_y<0) min_y=0;
	if(max_y>199) max_y=199;

	gouraud_scan_edge(x1,y1,x2,y2,c1,c2);
	gouraud_scan_edge(x2,y2,x3,y3,c2,c3);
	gouraud_scan_edge(x3,y3,x4,y4,c3,c4);
	gouraud_scan_edge(x4,y4,x1,y1,c4,c1);

	for(i=min_y;i<max_y;i++)
		/*gouraud_line(polygon_edges[i][LEFT],polygon_edges[i][RIGHT],i,
						polygon_colors[i][LEFT],polygon_colors[i][RIGHT]);*/

		horizontal_shadow_smooth_line(polygon_edges[i][LEFT],polygon_edges[i][RIGHT],i,
					polygon_colors[i][LEFT],polygon_colors[i][RIGHT]);
	
}

void scan_edge(int x1,int y1,int x2,int y2)
{
	int i,deltax,x,xfixed;

	if(y1==y2) return;

	if(y1>y2)
	{
		xchg(y1,y2);
		xchg(x1,x2);
	}

	deltax=((x2-x1)<<7)/(y2-y1);
	xfixed=x1<<7;

	for(i=y1;i<y2;i++)
	{
		x=xfixed>>7;

		if(x<polygon_edges[i][LEFT]) polygon_edges[i][LEFT]=x;
		if(x>polygon_edges[i][RIGHT]) polygon_edges[i][RIGHT]=x;
		
		xfixed+=deltax;
	}
}

void polygon(int x1,int y1,int x2,int y2,int x3,
		int y3,int x4,int y4,unsigned char color)
{
	int min_y,max_y,i;

	for(i=0;i<200;i++)
	{
		polygon_edges[i][LEFT]=32000;
		polygon_edges[i][RIGHT]=-32000;
	}

	min_y=y1;
	if(y2<min_y) min_y=y2;
	if(y3<min_y) min_y=y3;
	if(y4<min_y) min_y=y4;

	max_y=y1;
	if(y2>max_y) max_y=y2;
	if(y3>max_y) max_y=y3;
	if(y4>max_y) max_y=y4;

	if(min_y<0) min_y=0;
	if(max_y>199) max_y=199;

	scan_edge(x1,y1,x2,y2);
	scan_edge(x2,y2,x3,y3);
	scan_edge(x3,y3,x4,y4);
	scan_edge(x4,y4,x1,y1);

	for(i=min_y;i<max_y;i++)
		//horizontal_line(polygon_edges[i][LEFT],polygon_edges[i][RIGHT],i,color);
		horizontal_shadow_line(polygon_edges[i][LEFT],polygon_edges[i][RIGHT],i,color);
}

void init_normals(void)
{
	int i;
	float x_normal,y_normal,z_normal,lenght;

	for(i=0;i<MAX_FACES;i++)
	{

		/*
			Xnormal=(P2.Y-P1.Y)(P1.Z-P3.Z)-(P2.Z-P1.Z)(P1.Y-P3.Y)
			Ynormal=(P2.Z-P1.Z)(P1.X-P3.X)-(P2.X-P1.X)(P1.Z-P3.Z)
			Znormal=(P2.X-P1.X)(P1.Y-P3.Y)-(P2.Y-P1.Y)(P1.X-P3.X)
		*/

		x_normal=((object[polygons[i].v2].y-object[polygons[i].v1].y)*
			(object[polygons[i].v1].z-object[polygons[i].v3].z))-
			((object[polygons[i].v2].z-object[polygons[i].v1].z)*
			(object[polygons[i].v1].y-object[polygons[i].v3].y));

		y_normal=((object[polygons[i].v2].z-object[polygons[i].v1].z)*
			(object[polygons[i].v1].x-object[polygons[i].v3].x))-
			((object[polygons[i].v2].x-object[polygons[i].v1].x)*
			(object[polygons[i].v1].z-object[polygons[i].v3].z));

		z_normal=((object[polygons[i].v2].x-object[polygons[i].v1].x)*
			(object[polygons[i].v1].y-object[polygons[i].v3].y))-
			((object[polygons[i].v2].y-object[polygons[i].v1].y)*
			(object[polygons[i].v1].x-object[polygons[i].v3].x));

		lenght=sqrt(x_normal*x_normal+y_normal*y_normal+z_normal*z_normal);
			
		normal[i].x=x_normal/lenght;
		normal[i].y=y_normal/lenght;
		normal[i].z=z_normal/lenght;
	}
}

void init_point_normals(void)
{
	int i,j,which_face[8],num;
	float x,y,z,lenght;

	for(i=0;i<MAX_POINTS;i++)
	{
		for(j=0,num=0;j<MAX_FACES;j++)
		{
			if(i==polygons[j].v1 || i==polygons[j].v2 ||
			i==polygons[j].v3 || i==polygons[j].v4)
			{
				which_face[num]=j;
				num++;
			}
		}

		for(j=0,x=0,y=0,z=0;j<num;j++)
		{
			x+=normal[which_face[j]].x;
			y+=normal[which_face[j]].y;
			z+=normal[which_face[j]].z;
		}

		x/=num;
		y/=num;
		z/=num;

		lenght=sqrt(x*x+y*y+z*z);

		point_normal[i].x=x/lenght;
		point_normal[i].y=y/lenght;
		point_normal[i].z=z/lenght;
	}	
}

void draw_object(int mode)
{
	int i,j;
	int x1,y1,x2,y2,x3,y3,x4,y4;
	float dot;
	unsigned char vcolors[4];
	int point_nr;
	int *vertexptr;

	for(i=0;i<MAX_FACES;i++)
	{
		x1=object_translated[polygons[i].v1].x;
		y1=object_translated[polygons[i].v1].y;
		x2=object_translated[polygons[i].v2].x;
		y2=object_translated[polygons[i].v2].y;
		x3=object_translated[polygons[i].v3].x;
		y3=object_translated[polygons[i].v3].y;
		x4=object_translated[polygons[i].v4].x;
		y4=object_translated[polygons[i].v4].y;

		if(((y1-y3)*(x2-x1)-(x1-x3)*(y2-y1))>0)
		{

			switch(mode)
			{
				case FILLED:
					polygon(x1,y1,x2,y2,x3,y3,x4,y4,polygons[i].color);
					break;
				case GOURAUD:

					vertexptr=&polygons[i].v1;
					for(j=0;j<4;j++)
					{
						point_nr=*vertexptr;

						dot=(point_normal_rotated[point_nr].x*light_rotated.x)+
							(point_normal_rotated[point_nr].y*light_rotated.y)+
							(point_normal_rotated[point_nr].z*light_rotated.z);
						
						if(dot>1 || dot<0) dot=0;
						vcolors[j]=(dot*62)+1;
					
						vertexptr++;
					}

					gouraud_polygon(x1,y1,x2,y2,x3,y3,x4,y4,
									vcolors[0],vcolors[1],vcolors[2],vcolors[3]);
					break;
				case TEXTURED:
					textured_polygon(x1,y1,x2,y2,x3,y3,x4,y4,
									127,0,0,0,0,127,127,127,pol_texture);
			}
		}
	}
}

void scale_object(float scale)
{
	int i;

	for(i=0;i<MAX_POINTS;i++)
	{
		object[i].x*=scale;
		object[i].y*=scale;
		object[i].z*=scale;
	}
}

void translate_object(void)
{
	int i;

	for(i=0;i<MAX_POINTS;i++)
		translate_point(&object_translated[i],&object_rotated[i]);
}

void rotate_object(unsigned angle_x,unsigned angle_y,unsigned angle_z)
{
	int i;

	angle_x&=(MAX_DEGS-1);
	angle_y&=(MAX_DEGS-1);
	angle_z&=(MAX_DEGS-1);

	for(i=0;i<MAX_POINTS;i++)
		rotate_point(&object_rotated[i],&object[i],angle_x,angle_y,angle_z);

	// rotate normals

	/*for(i=0;i<MAX_FACES;i++)
		rotate_point(&normal_rotated[i],&normal[i],angle_x,angle_y,angle_z);*/

	// rotate point-normals

	for(i=0;i<MAX_POINTS;i++)
		rotate_point(&point_normal_rotated[i],&point_normal[i],angle_x,angle_y,angle_z);

	light_rotated.x=light.x;
	light_rotated.y=light.y;
	light_rotated.z=light.z;
}

void init_background(void)
{
	asm {
		push es
		les di,background
		mov dx,200 }
ver:
	asm {
		mov cx,320 }
hor:
	asm {
		mov ax,cx
		xor ax,dx
		and al,63
		add al,128+64
		stosb
		dec cx
		jnz hor
		dec dx
		jnz ver		
		pop es }
}

void recover_background(void)
{
	asm {
		push ds
		push es
		les di,frame_buffer
		lds si,background
		mov cx,16000
		cld
		db 66h
		rep movsw
		pop es
		pop ds }
}

int main(void)
{
	int ax=0,ay=0,az=0;
	//unsigned long frames;
	//time_t before,after;
	int mode=0;

	clrscr();

	if((frame_buffer=(unsigned char *)malloc(64000))==NULL ||
	(background=(unsigned char *)malloc(64000))==NULL)
	{
		free(frame_buffer);
		printf("%s","Not enough memory!\n\n");
		return 1;
	}

	init_sincos();
	//scale_object(50.0);

	make_light_unitvector(0,0,-100,0,0,0);
	init_normals();
	init_point_normals();

	init_background();
	set_vga();
	set_palette2();

	//before=time(NULL);
	//frames=0;

	while(mode<3)
	{
		rotate_object(ax,ay,az);
		translate_object();

		draw_object(mode);

		retrace();
		copy_buffer();

		if(mode==FILLED || mode==GOURAUD)
			recover_background();
		else
			clear_buffer();

		ax++;
		ay+=3;
		az++;

		if(kbhit())
		{
			getch();
			mode++;

			if(mode==GOURAUD)
				set_palette_light();
			else if(mode==TEXTURED)
				init_pol_texture();
		}
	}

	//after=time(NULL);

	set_text();
	free(background);
	free(frame_buffer);

	//printf("%s%f","Average framerate was: ",(float)(frames/(after-before)));

	return 0;
}