/* 320x200x64k VESA mode */

#include <dpmi.h>
#include <sys/nearptr.h>

#define V_MODE 0x10e
#define BUFFER_SIZE 128000
#define BANK_SIZE 65536

#define COLOR16(r,g,b) (short)((r<<11)|(g<<5)|b)
#define GET_R(c) (unsigned char)(c>>11)
#define GET_G(c) (unsigned char)((c>>5)&63)
#define GET_B(c) (unsigned char)(c&31)

unsigned char *frame_buffer;

int enable_graph(void)
{
	__dpmi_regs r;

	frame_buffer=(unsigned char *)malloc(BUFFER_SIZE);
	if(!frame_buffer) return 0;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x4f02;
	r.x.bx=V_MODE;
	__dpmi_int(0x10,&r);

	return 1;
}

void disable_graph(void)
{
	__dpmi_regs r;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x03;
	__dpmi_int(0x10,&r);

	free(frame_buffer);
}

void set_vesa_bank(int b_nr)
{
	__dpmi_regs r;

	memset(&r,0,sizeof(__dpmi_regs));
	r.x.ax=0x4f05;
	r.x.bx=0;
	r.x.dx=b_nr;
	__dpmi_int(0x10,&r);
}

void clear_buffer(void)
{
	memset(frame_buffer,0,BUFFER_SIZE);
}

void copy_buffer(void)
{
	unsigned char *src,*dest;

	__djgpp_nearptr_enable();

	src=(unsigned char *)frame_buffer;
	dest=(unsigned char *)0x0a0000+__djgpp_conventional_base;
	
	set_vesa_bank(0);
	memcpy(dest,src,BANK_SIZE);
	set_vesa_bank(1);
	memcpy(dest,src+BANK_SIZE,BUFFER_SIZE-BANK_SIZE);

	__djgpp_nearptr_disable();
}

void retrace(void)
{
	while(!(inp(0x03da)&8));
	while((inp(0x03da)&8));
}