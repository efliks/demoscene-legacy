

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _PCXFILES_H
#define _PCXFILES_H

typedef struct tPCXHEAD
{
	BYTE	Manufacter; // 0x0a
	BYTE	Version;
	BYTE	Encoding; // 0x01
	BYTE	BitsPerPixel;
	WORD	XMin;
	WORD	YMin;
	WORD	XMax;
	WORD	YMax;
	WORD	VertDPI;
	BYTE	Palette[48];
	BYTE	Reserved;
	BYTE	ColorPlanes;
	WORD	BytesPerLine;
	WORD	PaletteType;
	WORD	HScrSize;
	WORD	VScrSize;
	BYTE	Filler[56];
} PCXHEAD;

//----------------------------------------------------------------------

int SavePCX(char *FileName, BYTE *Buffer, BYTE *Palette, WORD Width, WORD Height);

#endif
