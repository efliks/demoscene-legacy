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

unsigned char font[2048];


void init_font(void)
{
    __dpmi_regs r;
    long p_addr;
    unsigned char* font_addr;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x1130;
    r.x.bx = 0x0300;
    __dpmi_int(0x10, &r);

    p_addr = ((r.x.es) << 4) + (r.x.bp);
    __djgpp_nearptr_enable();
    font_addr = (unsigned char*)p_addr + __djgpp_conventional_base;
    memcpy(font, font_addr, 2048);
    __djgpp_nearptr_disable();
}

void put_char(char ascii_code,int x,int y,
		unsigned char color,unsigned char *where)
{
	unsigned char *off,*char_off,curr_line;
	int i,j;

	off=where;
	off+=((y<<6)+(y<<8)+x);

	char_off=font;
	char_off+=ascii_code<<3;

	for(i=0;i<8;i++)
	{
		curr_line=*char_off;
		
		for(j=7;j>=0;j--)
		{
			if((curr_line>>j)&1) *off=color;
			off++;
		}

                off+=312;
		char_off++;
		color++;
	}
}

void put_string(char* text, int x, int y,
    unsigned char color, unsigned char* where)
{
    char* s;

    s = text;
    while (*s) {
        put_char(*s, x, y, color, where);
        s++;
        x += 8;
    }
}
