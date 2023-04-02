#ifndef _GRAPH_H
#define _GRAPH_H

#define BUFFER_SIZE 64000


void set_vga(void);
void set_text(void);

void copy_buffer(void);
void clear_buffer(void);

void retrace(void);

void set_palette(void);
void get_palette(void);

void soft_screen(void);

extern unsigned char *frame_buffer, palette[768];

#endif  // _GRAPH_H
