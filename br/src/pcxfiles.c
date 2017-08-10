

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


BYTE def_header[128]={10,5,1,8,0,0,0,0,63,1,199,0,64,1,200,
	0,254,36,242,191,119,115,70,0,0,0,0,0,118,7,0,0,124,6,0,0,144,244,124,0,172,
	240,124,0,24,0,0,0,197,185,247,191,24,0,0,0,0,0,0,0,2,0,0,0,1,1,64,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//----------------------------------------------------------------------

int SavePCX(char *FileName, BYTE *Buffer, BYTE *Palette, WORD Width, WORD Height) {
	PCXHEAD Head;
	DWORD i, j, FileSize, ImgSize = Width * Height;
	FILE *FPtr;
	BYTE c, cc, n_found;

	memcpy(&Head, def_header, sizeof(PCXHEAD));

	Head.XMax = Width - 1;
	Head.YMax = Height - 1;
	Head.VertDPI = Height;
	Head.BytesPerLine = Width;

	FPtr = fopen(FileName, "w+b");
	if (!FPtr) return 0;

	fwrite(&Head, sizeof(PCXHEAD), 1, FPtr);

	i = 0;
	while (i != ImgSize) {
		c = Buffer[i];

		if (c >= 192) {
			cc = 192 | 1;
			fputc(cc, FPtr);
			fputc(c, FPtr);
			i++;
			continue;
		}

		n_found = 0;
		for (j = 1; j < 63; j++) {
			if ((i + j) == ImgSize) break;
			
			if (Buffer[i + j] == c)
				n_found++;
			else
				break;
		}

		if (n_found) {
			cc = 192 | n_found;
			fputc(cc, FPtr);
			fputc(c, FPtr);
			i += n_found;
		}
		else {
			fputc(c, FPtr);
			i++;
		}
	}

	for (i = 0; i < 768; i++) {
		fputc(Palette[i] << 2, FPtr);
	}

	FileSize = ftell(FPtr);
	fclose(FPtr);

	return FileSize;
}
