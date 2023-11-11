#ifndef _GFX_H
#define _GFX_H

void set_vga();
void set_text();

void copy_buffer(unsigned char* buffer);
void clear_buffer(unsigned char* buffer);

void retrace();

void set_palette(unsigned char* my_pal);
void dump_palette(unsigned char* my_pal);

void draw_sprite(unsigned char* sprite, int x, int y, int width, int height, unsigned char* buffer);

int is_key_pressed();

#endif // _GFX_H
