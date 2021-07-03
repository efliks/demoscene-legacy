
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _vga_h
#define _vga_h

//------------------------------------------------------------

extern BYTE *FrameBuffer, *VideoMem, MainPalette[768];
extern int FadeFlag;

extern int WindowX1, WindowY1, WindowX2, WindowY2;

//extern void CopyFrameBuffer(void);

//------------------------------------------------------------

#define SetVGA() { _AX = 0x13; geninterrupt(0x10); }
#define SetText() { _AX = 0x03; geninterrupt(0x10); }

#define Retrace() { while(!(inp(0x03da) & 8)); while((inp(0x03da) & 8)); }

#define ClearFrameBuffer() { memset(FrameBuffer, 0, 64000); }
//#define CopyFrameBuffer() { memcpy(VideoMem, FrameBuffer, 64000); }

#define CopyFrameBuffer() { asm { push ds; push es; lds si, [FrameBuffer]; \
	mov ax, 0a000h; mov es, ax; xor di, di; mov cx, 64000 / 4; cld; db 66h; \
	rep movsw; pop es; pop ds; } }

#define FreeFrameBuffer() { free(FrameBuffer); }

//------------------------------------------------------------

int SetFrameBuffer(void);
void SetPalette(BYTE *pal);
void GetPalette(BYTE *pal);
int LoadPalette(char *filename, BYTE *pal);
void DrawFrame(int x1, int y1, int x2, int y2, BYTE color);
void PutImage(BYTE *spr, int x, int y, int w, int h, int safe, int transp);
void DrawLine(int x1, int y1, int x2, int y2, BYTE color);
void SetWindow(int x1, int y1, int x2, int y2);
void FadeDown(void);
void FadeUp(void);

#endif
