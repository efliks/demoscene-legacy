
DWORD SavePCX(char *FileName, BYTE *Buffer, BYTE *Palette, WORD Width, WORD Height) {
	PCXHEAD		Head;
	DWORD		i, j, FileSize, ImgSize = Width * Height;
	FILE		*FPtr;
	BYTE		c, cc, n_found;

	memset(&Head, 0, sizeof(PCXHEAD));
	Head.Manufacter		= 0x0a;
	Head.Version		= 5;
	Head.Encoding		= 1;
	Head.BitsPerPixel	= 8;
	Head.XMin			= 0;
	Head.YMin			= 0;
	Head.XMax			= Width - 1;
	Head.YMax			= Height - 1;
	Head.HRes			= Width;
	Head.VRes			= Height;
	Head.NumOfPlanes	= 1;
	Head.BytesPerLine	= Width;
	Head.PaletteType	= 1;
	Head.HSize			= Width;
	Head.VSize			= Height;


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
