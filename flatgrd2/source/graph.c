/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

unsigned char *frame_buffer,palette[768];

void set_vga(void)
{
	__dpmi_regs r;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x13;
	__dpmi_int(0x10,&r);
}

void set_text(void)
{
	__dpmi_regs r;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x03;
	__dpmi_int(0x10,&r);
}

void copy_buffer(void)
{
	unsigned char *screen;
	
	__djgpp_nearptr_enable();
	screen=(unsigned char *)0x0a0000+__djgpp_conventional_base;
	memcpy(screen,frame_buffer,64000);
	__djgpp_nearptr_disable();
}

void clear_buffer(void)
{
	memset(frame_buffer,0,64000);
}

void retrace(void)
{
	while(!(inp(0x03da)&8));
	while((inp(0x03da)&8));
}

void set_palette(void)
{
	int i;

	outp(0x03c8,0);
	for(i=0;i<768;i++) outp(0x03c9,palette[i]);
}

void get_palette(void)
{
	int i;

	outp(0x03c7,0);
	for(i=0;i<768;i++) palette[i]=inp(0x03c9);
}