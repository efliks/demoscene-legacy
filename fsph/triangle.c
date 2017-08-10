#define SHIFT_CONST 6
#define xchg(a,b) { asm { mov ax,a; xchg b,ax; mov a,ax } }

void horizontal_line(int x1,int x2,int y,unsigned char color,
						unsigned char *buffer)
{
	asm {
		mov ax,x1
		cmp ax,x2
		je quit
		jl ok
		xchg ax,x2
		mov x1,ax }
ok:
	asm {
		push es
		les di,buffer
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

void triangle(int x1,int y1,int x2,int y2,int x3,int y3,
				unsigned char color,unsigned char *buffer)
{
	int dx12,dx13,dx23,fixedx1,fixedx2,i;
	int line_x1,line_x2;

	if(y1>y2)
	{
		xchg(y1,y2);
		xchg(x1,x2);
	}
	if(y2>y3)
	{
		xchg(y2,y3);
		xchg(x2,x3);
	}
	if(y1>y2)
	{
		xchg(y1,y2);
		xchg(x1,x2);
	}

	if((i=y2-y1)!=0)
		dx12=((x2-x1)<<SHIFT_CONST)/i;
	else
		dx12=0;

	if((i=y3-y1)!=0)
		dx13=((x3-x1)<<SHIFT_CONST)/i;
	else
		dx13=0;

	if((i=y3-y2)!=0)
		dx23=((x3-x2)<<SHIFT_CONST)/i;
	else
		dx23=0;

	fixedx1=fixedx2=x1<<SHIFT_CONST;
	for(i=y1;i<y2;i++)
	{
		if(i>=0 && i<=199)
		{
			line_x1=fixedx1>>SHIFT_CONST;
			line_x2=fixedx2>>SHIFT_CONST;

			if(line_x1<0) line_x1=0;
			else if(line_x1>319) line_x1=319;
			
			if(line_x2<0) line_x2=0;
			else if(line_x2>319) line_x2=319;

			horizontal_line(line_x1,line_x2,i,color,buffer);
		}

		fixedx1+=dx13;
		fixedx2+=dx12;
	}
	
	fixedx2=x2<<SHIFT_CONST;
	for(i=y2;i<y3;i++)
	{
		if(i>=0 && i<=199)
		{
			line_x1=fixedx1>>SHIFT_CONST;
			line_x2=fixedx2>>SHIFT_CONST;

			if(line_x1<0) line_x1=0;
			else if(line_x1>319) line_x1=319;
			
			if(line_x2<0) line_x2=0;
			else if(line_x2>319) line_x2=319;

			horizontal_line(line_x1,line_x2,i,color,buffer);
		}

		fixedx1+=dx13;
		fixedx2+=dx23;
	}
}