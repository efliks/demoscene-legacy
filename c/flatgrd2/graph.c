/*
 * FlatGrd2 06/02/02
 * Mikolaj Felix a.k.a. Majuma
 * mfelix@polbox.com
 */

#include <dos.h>
#include <mem.h>
#include <dpmi.h>
#include <string.h>  // for memset
#include <sys/nearptr.h>

unsigned char *frame_buffer, palette[768];


void set_vga(void)
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x13;
    __dpmi_int(0x10, &r);
}

void set_text(void)
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x03;
    __dpmi_int(0x10, &r);
}

void copy_buffer(void)
{
    unsigned char* screen;

    __djgpp_nearptr_enable();
    screen = (unsigned char*)0x0a0000 + __djgpp_conventional_base;
    memcpy(screen, frame_buffer, 64000);
    __djgpp_nearptr_disable();
}

void clear_buffer(void)
{
    memset(frame_buffer, 0, 64000);
}

void retrace(void)
{
    while (!(inp(0x03da) & 8))
        ;
    while ((inp(0x03da) & 8))
        ;
}

void set_palette(void)
{
    int i;

    outp(0x03c8, 0);
    for (i = 0; i < 768; i++) {
        outp(0x03c9, palette[i]);
    }
}

void get_palette(void)
{
    int i;

    outp(0x03c7, 0);
    for (i = 0; i < 768; i++) {
        palette[i] = inp(0x03c9);
    }
}

void soft_screen(void)
{
	unsigned char *ptr = frame_buffer;
	unsigned i, color;

	memset(ptr, 0, 320);
	ptr += 320;

	for(i = 0; i < 64000-320*2; i++)
	{
		color = *(ptr-1) + *(ptr+1) + *(ptr-320) + *(ptr+320);
		color >>= 2;
		*ptr = (unsigned char)color;
		ptr++;
	}

	memset(ptr, 0, 320);			
}

