/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

void gouraud_line(int x1,int x2,int y,unsigned char c1,unsigned char c2,
				unsigned char *buffer)
{
	/*
		x1	ebp+08
		x2	ebp+12
		y	ebp+16
		c1	ebp+20
		c2	ebp+24
	*/

	asm volatile("movl 16(%%ebp),%%eax;
		orl %%eax,%%eax;
		jl gl_quit_proc;
		cmpl $199,%%eax;
		jg gl_quit_proc;

		movl 8(%%ebp),%%eax;
		cmpl 12(%%ebp),%%eax;
		je gl_quit_proc;
		jl gl_no_xchg;

		xchgl 12(%%ebp),%%eax;
		movl %%eax,8(%%ebp);

		movl 20(%%ebp),%%eax;
		xchgl 24(%%ebp),%%eax;
		movl %%eax,20(%%ebp);
gl_no_xchg:
		mov 16(%%ebp),%%eax;
		movl %%eax,%%ecx;
		sall $6,%%eax;
		sall $8,%%ecx;
		addl %%ecx,%%eax;
		addl 8(%%ebp),%%eax;
		addl %%eax,%%edi;

		movl 12(%%ebp),%%ecx;
		subl 8(%%ebp),%%ecx;

		movl 24(%%ebp),%%eax;
		subl 20(%%ebp),%%eax;
		sall $8,%%eax;
		cltd;
		idivl %%ecx;
		movl %%eax,%%edx;

		movl 20(%%ebp),%%ebx;
		sall $8,%%ebx;
gl_draw:
		movl %%ebx,%%eax;
		sarl $8,%%eax;
		stosb;

		addl %%edx,%%ebx;
		decl %%ecx;
		jnz gl_draw;
gl_quit_proc:"
		:
		: "D" (frame_buffer)
		: "%eax", "%ebx", "%ecx", "%edx", "cc", "memory");
}

/*
void gouraud_line(int x1,int x2,int y,unsigned char c1,unsigned char c2,
				unsigned char *buffer)
{
	long delta_c,curr_c;
	int i;
	unsigned char *ofs;

	if(y<0 || y>199 || x1==x2) return;

	if(x1>x2)
	{
		i=x1;
		x1=x2;
		x2=i;

		i=c1;
		c1=c2;
		c2=i;
	}

	delta_c=((c2-c1)<<SHIFT_CONST)/(x2-x1);
	curr_c=c1<<SHIFT_CONST;

	ofs=buffer;
	ofs+=((y<<6)+(y<<8)+x1);

	i=0;
	while((i++)<(x2-x1))
	{
		*ofs=curr_c>>SHIFT_CONST;
		curr_c+=delta_c;
		ofs++;
	}
}
*/

void gouraud_triangle(int x1,int y1,int x2,int y2,int x3,int y3,
				unsigned char c1,unsigned char c2,unsigned char c3,
				unsigned char *buffer)
{
	long dx12,dx13,dx23,scan_x1,scan_x2;
	long dc12,dc13,dc23,scan_c1,scan_c2;
	int i;

	if(y1>y2)
	{
		i=y1;
		y1=y2;
		y2=i;

		i=x1;
		x1=x2;
		x2=i;

		i=c1;
		c1=c2;
		c2=i;
	}
	if(y2>y3)
	{
		i=y2;
		y2=y3;
		y3=i;

		i=x2;
		x2=x3;
		x3=i;

		i=c2;
		c2=c3;
		c3=i;
	}
	if(y1>y2)
	{
		i=y1;
		y1=y2;
		y2=i;

		i=x1;
		x1=x2;
		x2=i;

		i=c1;
		c1=c2;
		c2=i;
	}

	if((i=y2-y1)!=0)
	{
		dx12=((x2-x1)<<SHIFT_CONST)/i;
		dc12=((c2-c1)<<SHIFT_CONST)/i;
	}
	else
	{
		dx12=0;
		dc12=0;
	}

	if((i=y3-y1)!=0)
	{
		dx13=((x3-x1)<<SHIFT_CONST)/i;
		dc13=((c3-c1)<<SHIFT_CONST)/i;
	}
	else
	{
		dx13=0;
		dc13=0;
	}

	if((i=y3-y2)!=0)
	{
		dx23=((x3-x2)<<SHIFT_CONST)/i;
		dc23=((c3-c2)<<SHIFT_CONST)/i;
	}
	else
	{
		dx23=0;
		dc23=0;
	}

	scan_x1=scan_x2=x1<<SHIFT_CONST;
	scan_c1=scan_c2=c1<<SHIFT_CONST;

	for(i=y1;i<y2;i++)
	{
		gouraud_line(scan_x1>>SHIFT_CONST,scan_x2>>SHIFT_CONST,i,
					scan_c1>>SHIFT_CONST,scan_c2>>SHIFT_CONST,buffer);

		scan_x1+=dx13;
		scan_x2+=dx12;
		scan_c1+=dc13;
		scan_c2+=dc12;
	}
	
	scan_x2=x2<<SHIFT_CONST;
	scan_c2=c2<<SHIFT_CONST;

	for(i=y2;i<y3;i++)
	{
		gouraud_line(scan_x1>>SHIFT_CONST,scan_x2>>SHIFT_CONST,i,
					scan_c1>>SHIFT_CONST,scan_c2>>SHIFT_CONST,buffer);

		scan_x1+=dx13;
		scan_x2+=dx23;
		scan_c1+=dc13;
		scan_c2+=dc23;
	}
}