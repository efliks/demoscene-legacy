/*****************************************
  Mroczna Harmonia

  gfx.c   xxxxxxxxxxxx

  (C) 2001, 2023  M. Feliks
*****************************************/

#include <dos.h>
#include <string.h> // for memset


void set_vga()
{
    _AX = 0x13;
    geninterrupt(0x10);
}

void set_text()
{
    _AX = 0x3;
    geninterrupt(0x10);
}

void copy_buffer(unsigned char* buffer)
{
    unsigned char far* ptr_vidmem = (unsigned char far *)0xA0000000L;
    memcpy(ptr_vidmem, buffer, 64000);
}

void clear_buffer(unsigned char* buffer)
{
    memset(buffer, 0, 64000);
}

void retrace()
{
    while (!(inportb(0x03da) & 8))
        ;
    while ((inportb(0x03da) & 8))
        ;
}

void set_palette(unsigned char* my_pal)
{
    int i;
        
    outportb(0x03c8, 0);
    for (i = 0; i < 768; i++) {
        outportb(0x03c9, my_pal[i]);
    }
}

void dump_palette(unsigned char* my_pal)
{
    int i;
        
    outportb(0x03c7, 0);
    for (i = 0; i < 768; i++) {
        my_pal[i] = inportb(0x03c9);
    }
}

// kbhit() from conio.h does not work properly in VirtualBox
int is_key_pressed()
{
    if (inportb(0x60) > 1) {
        return 0;
    }
    
    return 1;
}

// end of low-level code

void draw_sprite(unsigned char* sprite, int x, int y, int width, int height, unsigned char* buffer)
{
    int i, j;
    
    unsigned char* sprptr = sprite;
    unsigned char* bptr = buffer + x + y * 320;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++, sprptr++, bptr++) {
            if (*sprptr != 0) {
                *bptr = *sprptr;
            }
        }
        bptr += 320 - width;
    }
}
