unsigned char font[2048];

void init_font(void)
{
	__dpmi_regs r;
	long p_addr;
	unsigned char *font_addr;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x1130;
	r.x.bx=0x0300;
	__dpmi_int(0x10,&r);

	p_addr=((r.x.es)<<4)+(r.x.bp);
	__djgpp_nearptr_enable();
	font_addr=(unsigned char *)p_addr+__djgpp_conventional_base;
	memcpy(font,font_addr,2048);
	__djgpp_nearptr_disable();
}

void put_char(char ascii_code,int x,int y,short color)
{
	/*
		ac	ebp+8
		x	ebp+12
		y	ebp+16
		col	ebp+20
	*/

	asm volatile("movl 16(%%ebp),%%eax;
		movl %%eax,%%edx;
		sall $6,%%eax;
		sall $8,%%edx;
		addl %%edx,%%eax;
		addl 12(%%ebp),%%eax;
		sall $1,%%eax;
		addl %%eax,%%edi;

		movl 8(%%ebp),%%eax;
		sall $3,%%eax;
		addl %%eax,%%esi;

		movl 20(%%ebp),%%edx;

		movb $8,%%ch;
pc_hor:
		lodsb;
		movb $8,%%cl;
pc_ver:
		rclb $1,%%al;
		jae pc_next;
		movw %%dx,(%%edi);
pc_next:
		addl $2,%%edi;
		decb %%cl;
		jnz pc_ver;

		addl $((320-8)*2),%%edi;
		decb %%ch;
		jnz pc_hor"
		:
		: "S" (font), "D" (frame_buffer)
		: "%eax", "%ecx", "%edx", "cc", "memory");
}

/*
void put_char(char ascii_code,int x,int y,short color)
{
	unsigned char *char_ofs,curr_line;
	int i,j;
	short *ofs;

	ofs=(short *)frame_buffer;
	ofs+=((y<<6)+(y<<8)+x);

	char_ofs=font;
	char_ofs+=ascii_code<<3;

	for(i=0;i<8;i++)
	{
		curr_line=*char_ofs;
		for(j=7;j>=0;j--)
		{
			if((curr_line>>j)&1) *ofs=color;
			ofs++;
		}
		ofs+=312;
		char_ofs++;
	}
}
*/

void put_string(char *string,int x,int y,unsigned color)
{
	char *s;

	s=string;
	while(*s)
	{
		put_char(*s,x,y,color);
		s++;
		x+=8;
	}
}