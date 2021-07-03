
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


BYTE *FrameBuffer, *VideoMem = MK_FP(0x0a000, 0), MainPalette[768];
int FadeFlag = 0;

int WindowX1 = 0, WindowY1 = 0, WindowX2 = 240, WindowY2 = 200;

//------------------------------------------------------------

int SetFrameBuffer(void) {
	FrameBuffer = (BYTE *)malloc(64000);
	if (!FrameBuffer) {
		return 0;
	}

	ClearFrameBuffer();

	return 1;
}

//------------------------------------------------------------

void SetPalette(BYTE *pal) {
	int i;

	outp(0x03c8, 0);

	for (i = 0; i < 768; i++) {
		outp(0x03c9, pal[i]);
	}
}

void GetPalette(BYTE *pal) {
	int i;

	outp(0x03c7, 0);

	for (i = 0; i < 768; i++) {
		pal[i] = inp(0x03c9);
	}
}

int LoadPalette(char *filename, BYTE *pal) {
	FILE *p;

	p = fopen(filename, "r+b");
	if (!p) {
		return 0;
	}

	fread(pal, 1, 768, p);
	fclose(p);

	return 1;
}

//------------------------------------------------------------

void PutImage(BYTE *spr, int x, int y, int w, int h, int safe, int transp) {
	BYTE *s = spr, *d = FrameBuffer;
	int i, j;
	int add_x, add_y, max_x, max_y, add_s, add_d;

	if (!safe) {
		d += (x + (y << 6) + (y << 8));

		if (transp) {
			add_d = 320 - w;

			for (i = 0; i < h; i++) {
				for (j = 0; j < w; j++, s++, d++) {
					if (*s) *d = *s;
				}
				d += add_d;
			}
		}
		else {
			for (i = 0; i < h; i++, s += w, d += 320) {
				memcpy(d, s, w);
			}
		}
	}
	else {
		add_x = add_y = 0;
		max_x = x + w;
		max_y = y + h;

		if (x < WindowX1) {
			if (x < (WindowX1 - w)) return;
			add_x = WindowX1 - x;
			x = WindowX1;
		}
		else if (x > (WindowX2 - 1 - w)) {
			if (x > (WindowX2 - 1)) return;
			max_x = WindowX2;
		}

		if (y < WindowY1) {
			if (y < (WindowY1 - h)) return;
			add_y = WindowY1 - y;
			y = WindowY1;
		}
		else if (y > (WindowY2 - 1 - h)) {
			if (y > (WindowY2 - 1)) return;
			max_y = WindowY2;
		}

		d += (x + (y << 6) + (y << 8));
		add_d = 320 - (max_x - x);

		s += (add_x + add_y * w);
		add_s = w - (max_x - x);
	
		if (transp) {
			for (i = y; i < max_y; i++) {
				for (j = x; j < max_x; j++, s++, d++) {
					if (*s) *d = *s;
				}
				d += add_d;
				s += add_s;
			}
		}
		else {
			for (i = y; i < max_y; i++) {
				for (j = x; j < max_x; j++, s++, d++) {
					*d = *s;
				}
				d += add_d;
				s += add_s;
			}
		}
	}
}

//------------------------------------------------------------

void DrawFrame(int x1, int y1, int x2, int y2, BYTE color) {
	int i;
	BYTE *fb1, *fb2;

	fb1 = FrameBuffer + x1 + y1 * 320;
	fb2 = FrameBuffer + x1 + y2 * 320;

	for (i = x1; i <= x2; i++) {
		*fb1 = color;
		fb1++;
		*fb2 = color;
		fb2++;
	}

	fb1 = FrameBuffer + x1 + y1 * 320;
	fb2 = FrameBuffer + x2 + y1 * 320;

	for (i = y1; i <= y2; i++) {
		*fb1 = color;
		fb1 += 320;

		*fb2 = color;
		fb2 += 320;
	}
}

//------------------------------------------------------------

void DrawLine(int x1, int y1, int x2, int y2, BYTE color) {
	long Delta, Len, X, Y;
	int i;
	BYTE *p = FrameBuffer;

	if (x1 == x2) {
		if (y1 == y2) {
			*(p + x1 + y1 * 320) = color;
			return;
		}

		// line is vertical

		if (y1 > y2) {
			i = y1;
			y1 = y2;
			y2 = i;
		}

		Len = y2 - y1;
		for (i = 0; i <= Len; i++) *(p + x1 + (y1 + i) * 320) = color;

		return;
	}

	if (y1 == y2) {
		if (x1 == x2) {
			*(p + x1 + y1 * 320) = color;
			return;
		}

		// line is horizontal

		if (x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
		}

		Len = x2 - x1;
		for (i = 0; i <= Len; i++) *(p + x1 + i + y1 * 320) = color;

		return;
	}

	if (abs(x2 - x1) > abs(y2 - y1)) {
		// line is more horizontal

		if (x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i;
		}

		Len = x2 - x1;
		Delta = ((y2 - y1) << 8) / Len;
		Y = y1;
		Y <<= 8;

		for (i = 0; i <= Len; i++) {
			*(p + x1 + i + (Y >> 8) * 320) = color;
			Y += Delta;
		}

		return;
	}
	else {
		// line is more vertical

		if (y1 > y2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i;
		}

		Len = y2 - y1;
		Delta = ((x2 - x1) << 8) / Len;
		X = x1;
		X <<= 8;

		for (i = 0; i <= Len; i++) {
			*(p + (X >> 8) + (y1 + i) * 320) = color;
			X += Delta;
		}
		
		return;
	}
}

//------------------------------------------------------------

void SetWindow(int x1, int y1, int x2, int y2) {
	WindowX1 = x1;
	WindowY1 = y1;
	WindowX2 = x2;
	WindowY2 = y2;
}

//------------------------------------------------------------

void FadeDown(void) {
	int i, j;
	BYTE TmpPal[768], t;

	if (!FadeFlag) return;

	GetPalette(TmpPal);

	for (i = 0; i < 64; i++) {
		for (j = 0; j < 768; j++) {
			t = TmpPal[j];
			if (t > 0) t--;
			TmpPal[j] = t;
		}

		SetPalette(TmpPal);
		//Retrace();
		Wait(1);
	}
}

void FadeUp(void) {
	int i, j;
	BYTE TmpPal[768], t;

	if (!FadeFlag) return;

	GetPalette(TmpPal);

	for (i = 0; i < 64; i++) {
		for (j = 0; j < 768; j++) {
			t = TmpPal[j];
			if (t < MainPalette[j]) t++;
			TmpPal[j] = t;
		}

		SetPalette(TmpPal);
		//Retrace();
		Wait(1);
	}
}

