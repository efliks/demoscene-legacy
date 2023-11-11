#ifndef _FONT_H
#define _FONT_H

void init_font(void);

void put_char(char ascii_code, int x, int y, unsigned char color, unsigned char* where);
void put_string(char* text, int x, int y, unsigned char color, unsigned char* where);

#endif // _FONT_H
