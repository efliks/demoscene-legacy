

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */



int LoadBMP(char *Filename, BYTE *Buffer, BYTE *Palette,
	int ExpWidth, int ExpHeight) {

	FILE *p;
	BMPHEAD h;
	int i;
	BYTE *ptr;

	p = fopen(Filename, "r+b");
	if (!p) {
		return -1;
	}

	fread(&h, sizeof(BMPHEAD), 1, p);

	if (h.type[0] != 'B' || h.type[1] != 'M') {
		fclose(p);
		return -2;
	}

	if (ExpWidth != h.width || ExpHeight != h.height) {
		fclose(p);
		return -3;
	}

	if (h.bits != 8) {
		fclose(p);
		return -4;
	}

	if (Palette) {
		for (i = 0; i < 768; i += 3) {
			Palette[i + 2] = fgetc(p) >> 2;
			Palette[i + 1] = fgetc(p) >> 2;
			Palette[i + 0] = fgetc(p) >> 2;
			fgetc(p);
		}
	}
	else
		fseek(p, 1024, SEEK_CUR);


	ptr = Buffer + (ExpHeight - 1) * ExpWidth;
	for (i = 0; i < ExpHeight; i++) {
		fread(ptr, ExpWidth, 1, p);
		ptr -= ExpWidth;
	}

	fclose(p);

	return 1;
}

int SaveBMP(char *FileName, BYTE *Buffer, BYTE *Palette,
	int Width, int Height) {
	BMPHEAD Head;
	FILE *FPtr;
	int i;
	BYTE *Ptr;

	FPtr = fopen(FileName, "w+b");
	if (!FPtr) return 0;

	Head.type[0] = 'B';
	Head.type[1] = 'M';
	Head.size = sizeof(BMPHEAD) + 1024 + Width * Height;
	Head.res1 = Head.res2 = 0;
	Head.data_pos = sizeof(BMPHEAD) + 1024;
	Head.head_size = 40;
	Head.width = Width;
	Head.height = Height;
	Head.planes = 1;
	Head.bits = 8;
	Head.comp = 0;
	Head.data_size = Width * Height + 1024;
	Head.xpermeter = 0;
	Head.ypermeter = 0;
	Head.n_colors = 0;
	Head.col_imp = 0;

	fwrite(&Head, sizeof(BMPHEAD), 1, FPtr);

	for (i = 0; i < 768; i += 3) {
		fputc(Palette[i + 2] << 2, FPtr);
		fputc(Palette[i + 1] << 2, FPtr);
		fputc(Palette[i + 0] << 2, FPtr);
		fputc(0, FPtr);
	}

	Ptr = Buffer + (Height - 1) * Width;
	for (i = 0; i < Height; i++) {
		fwrite(Ptr, Width, 1, FPtr);
		Ptr -= Width;
	}

	fclose(FPtr);

	return 1;
}

//----------------------------------------------------------------------
