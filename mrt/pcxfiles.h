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
	WORD	HRes;
	WORD	VRes;
	BYTE	Palette[48];
	BYTE	Reserved;
	BYTE	NumOfPlanes;
	WORD	BytesPerLine;
	WORD	PaletteType;
	WORD	HSize;
	WORD	VSize;
	BYTE	Filler[54];
}PCXHEAD;

//----------------------------------------------------------------------

DWORD SavePCX(char *FileName, BYTE *Buffer, BYTE *Palette,
	WORD Width, WORD Height);

#endif
