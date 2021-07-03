/*
	Linear Texture Mapping Routines
	Mikolaj Felix 13/07/2001
	mfelix@polbox.com
*/

#define X1 0
#define Y1 1
#define X2 2
#define Y2 3

int polygon_texcoords[200][4];

void textured_line(int x1,int x2,int y,
			int tx1,int ty1,int tx2,int ty2,unsigned char *texture)
{
	unsigned delta_x,delta_y;

	asm {
		mov ax,x1
		cmp ax,x2
		jl ok
		je quit

		xchg ax,x2
		mov x1,ax
		
		mov ax,tx1
		xchg ax,tx2
		mov tx1,ax

		mov ax,ty1
		xchg ax,ty2
		mov ty1,ax }
ok:
	asm {
		push ds
		push es
		lds si,texture
		les di,frame_buffer
		
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x1
		add di,ax

		mov cx,x2
		sub cx,x1
		
		mov ax,tx2
		sub ax,tx1
		shl ax,7
		cwd
		idiv cx
		mov delta_x,ax

		mov ax,ty2
		sub ax,ty1
		shl ax,7
		cwd
		idiv cx
		mov delta_y,ax

		mov ax,tx1
		shl ax,7
		mov bx,ty1
		shl bx,7
		cld }
draw:
	asm {
		push ax
		push bx
		shr ax,7
		and bx,0ff80h
		add bx,ax
		mov al,byte ptr [si][bx] 
		stosb
		pop bx
		pop ax

		add ax,delta_x
		add bx,delta_y
		dec cx
		jnz draw

		pop es
		pop ds }
quit:
}

/*
void textured_line(int x1,int x2,int y,
					int tx1,int ty1,int tx2,int ty2,unsigned char *texture)
{
	int delta_x,delta_y,i,fixed_tx,fixed_ty,screen_off;

	if(x1==x2) return;
	if(x1>x2)
	{
		xchg(x1,x2);
		xchg(tx1,tx2);
		xchg(ty1,ty2);
	}

	delta_x=((tx2-tx1)<<7)/(x2-x1);
	delta_y=((ty2-ty1)<<7)/(x2-x1);
	fixed_tx=tx1<<7;
	fixed_ty=ty1<<7;

	screen_off=(y<<6)+(y<<8)+x1;

	for(i=x1;i<x2;i++)
	{
		frame_buffer[screen_off]=
			texture[(fixed_tx>>7)+(fixed_ty&0xff80)];

		fixed_tx+=delta_x;
		fixed_ty+=delta_y;
		screen_off++;
	}
}*/

void textured_edge(int x1,int y1,int x2,int y2,int tx1,int ty1,int tx2,int ty2)
{
	int i,deltax,xfixed,x;
	int delta_tx,delta_ty,scan_x,scan_y;

	if(y1==y2) return;

	if(y1>y2)
	{
		xchg(y1,y2);
		xchg(x1,x2);
		xchg(ty1,ty2);
		xchg(tx1,tx2);
	}

	deltax=((x2-x1)<<7)/(y2-y1);
	xfixed=x1<<7;

	delta_tx=((tx2-tx1)<<7)/(y2-y1);
	delta_ty=((ty2-ty1)<<7)/(y2-y1);
	scan_x=tx1<<7;
	scan_y=ty1<<7;

	for(i=y1;i<y2;i++)
	{
		x=xfixed>>7;

		if(x<polygon_edges[i][LEFT])
		{
			polygon_edges[i][LEFT]=x;
			polygon_texcoords[i][X1]=scan_x>>7;
			polygon_texcoords[i][Y1]=scan_y>>7;
		}		

		if(x>polygon_edges[i][RIGHT])
		{
			polygon_edges[i][RIGHT]=x;
			polygon_texcoords[i][X2]=scan_x>>7;
			polygon_texcoords[i][Y2]=scan_y>>7;
		}

		xfixed+=deltax;
		scan_x+=delta_tx;
		scan_y+=delta_ty;
	}
}

void textured_polygon(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,
						int tx1,int ty1,int tx2,int ty2,int tx3,int ty3,int tx4,int ty4,
						unsigned char *texture)
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

	textured_edge(x1,y1,x2,y2,tx1,ty1,tx2,ty2);
	textured_edge(x2,y2,x3,y3,tx2,ty2,tx3,ty3);
	textured_edge(x3,y3,x4,y4,tx3,ty3,tx4,ty4);
	textured_edge(x4,y4,x1,y1,tx4,ty4,tx1,ty1);

	for(i=min_y;i<max_y;i++)
	{
		textured_line(polygon_edges[i][LEFT],polygon_edges[i][RIGHT],i,
					polygon_texcoords[i][X1],polygon_texcoords[i][Y1],
					polygon_texcoords[i][X2],polygon_texcoords[i][Y2],texture);
	}
	
}