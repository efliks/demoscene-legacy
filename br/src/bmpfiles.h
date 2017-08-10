

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _bmphead_h
#define _bmphead_h

typedef struct sBMPHEAD
{
	BYTE	type[2];
	DWORD	size;
	WORD	res1, res2;
	DWORD	data_pos;
	DWORD	head_size;
	DWORD	width;
	DWORD	height;
	WORD	planes;
	WORD	bits;
	DWORD	comp;
	DWORD	data_size;
	DWORD	xpermeter;
	DWORD	ypermeter;
	DWORD	n_colors;
	DWORD	col_imp;
} BMPHEAD;

//----------------------------------------------------------------------

int LoadBMP(char *Filename, BYTE *Buffer, BYTE *Palette,
	int ExpWidth, int ExpHeight);

int SaveBMP(char *FileName, BYTE *Buffer, BYTE *Palette,
	int Width, int Height);


#endif
