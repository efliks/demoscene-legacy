
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


SPRITE SpriteData[MAX_SPRITES] = {{"statek1/Stat1_1.bmp", 20, 20, 0},
	{"statek1/Stat1_2.bmp", 20, 20, 0}, {"statek1/Stat1_3.bmp", 20, 20, 0}, 
	{"statek1/Stat1_4.bmp", 20, 20, 0}, {"statek1/Stat1_5.bmp", 20, 20, 0},
	{"statek1/Stat1_6.bmp", 20, 20, 0},

	{"statek2/Stat2_1.bmp", 20, 20, 0}, {"statek2/Stat2_2.bmp", 20, 20, 0},
	{"statek2/Stat2_3.bmp", 20, 20, 0}, {"statek2/Stat2_4.bmp", 20, 20, 0},
	{"statek2/Stat2_5.bmp", 20, 20, 0}, {"statek2/Stat2_6.bmp", 20, 20, 0},

	{"statek3/Stat3_1.bmp", 20, 20, 0}, {"statek3/Stat3_2.bmp", 20, 20, 0},
	{"statek3/Stat3_3.bmp", 20, 20, 0}, {"statek3/Stat3_4.bmp", 20, 20, 0},
	{"statek3/Stat3_5.bmp", 20, 20, 0}, {"statek3/Stat3_6.bmp", 20, 20, 0},

	{"statek4/Stat4_1.bmp", 20, 20, 0}, {"statek4/Stat4_2.bmp", 20, 20, 0},
	{"statek4/Stat4_3.bmp", 20, 20, 0}, {"statek4/Stat4_4.bmp", 20, 20, 0},
	{"statek4/Stat4_5.bmp", 20, 20, 0}, {"statek4/Stat4_6.bmp", 20, 20, 0},

	{"statek5/Stat5_1.bmp", 20, 20, 0}, {"statek5/Stat5_2.bmp", 20, 20, 0},
	{"statek5/Stat5_3.bmp", 20, 20, 0}, {"statek5/Stat5_4.bmp", 20, 20, 0},
	{"statek5/Stat5_5.bmp", 20, 20, 0}, {"statek5/Stat5_6.bmp", 20, 20, 0},

	{"statek6/Stat6_1.bmp", 20, 20, 0}, {"statek6/Stat6_2.bmp", 20, 20, 0},
	{"statek6/Stat6_3.bmp", 20, 20, 0}, {"statek6/Stat6_4.bmp", 20, 20, 0},
	{"statek6/Stat6_5.bmp", 20, 20, 0}, {"statek6/Stat6_6.bmp", 20, 20, 0},

	{"kapsula/kaps_1.bmp", 20, 20, 0}, {"kapsula/kaps_2.bmp", 20, 20, 0},
	{"kapsula/kaps_3.bmp", 20, 20, 0}, {"kapsula/kaps_4.bmp", 20, 20, 0},
	{"kapsula/kaps_5.bmp", 20, 20, 0}, {"kapsula/kaps_6.bmp", 20, 20, 0},
		
	{"dead1.bmp", 20, 20, 0},
	
	{"strzaly/ball.bmp", 8, 8, 0},
	{"strzaly/ball2.bmp", 8, 8, 0},
	{"strzaly/acid2.bmp", 8, 8, 0},
	{"strzaly/acid.bmp", 8, 8, 0},
	{"strzaly/buzz.bmp", 8, 8, 0},
	{"strzaly/buzz2.bmp", 8, 8, 0},
	{"strzaly/c_line.bmp", 8, 8, 0},
	{"strzaly/c_line2.bmp", 8, 8, 0},
	{"strzaly/job.bmp", 8, 8, 0},
	{"strzaly/job2.bmp", 8, 8, 0},
	{"strzaly/rail.bmp", 8, 8, 0},
	{"strzaly/rail2.bmp", 8, 8, 0},
	{"strzaly/spaw.bmp", 8, 8, 0},
	{"strzaly/spaw2.bmp", 8, 8, 0},
	{"strzaly/weak.bmp", 8, 8, 0},
	{"strzaly/weak2.bmp", 8, 8, 0},
		
	{"bum/bum_1.bmp", 20, 20, 0},
	{"bum/bum_2.bmp", 20, 20, 0},
	{"bum/bum_3.bmp", 20, 20, 0},
	{"bum/bum_4.bmp", 20, 20, 0},
	{"bum/bum_5.bmp", 20, 20, 0},
	{"bum/bum_6.bmp", 20, 20, 0}};


int LoadedSprites = 0;
char SprErrMsg[128];

//----------------------------------------------------------------------

int LoadSprite(SPRITE *Spr) {
	int i, r;
	char FullFile[64];

	if (Spr->Data) {
		return 1;
	}

	strcpy(FullFile, SPRITES_DIR);
	strcat(FullFile, Spr->Filename);

	strcpy(SprErrMsg, FullFile);

	Spr->Data = (BYTE *)malloc(Spr->Width * Spr->Height);
	if (!Spr->Data) {
		strcat(SprErrMsg, ": unable to alloc memory");

		FreeAllSprites();
		return 0;
	}

	r = LoadBMP(FullFile, Spr->Data, 0, Spr->Width, Spr->Height);
	if (r < 0) {

		switch (r) {
			case -1:
				strcat(SprErrMsg, ": file not found");
				break;
			case -2:
				strcat(SprErrMsg, ": format incorrect");
				break;
			case -3:
				strcat(SprErrMsg, ": format incorrect");
				break;
			case -4:
				strcat(SprErrMsg, ": unexpected size");
				break;
			default: //case -5:
				strcat(SprErrMsg, ": incorrect color depth");
		}
			
		FreeAllSprites();
		return 0;
	}

	return 1;
}

void FreeAllSprites(void) {
	int i;
	SPRITE *Spr = SpriteData;

	for (i = 0; i < MAX_SPRITES; i++, Spr++) {
		if (Spr->Data) {
			free(Spr->Data);
			Spr->Data = 0;
		}
	}
}

//----------------------------------------------------------------------

// only 20x20 sprites
void PutRotSprite(SPRITE *spr, int x, int y, int angle, int safe) {
	BYTE RotBuffer[28 * 28], ShadowBuffer[34 * 34], *ptr, *ptrs;
	int i, j, tx, ty, fy, sin, cos, sinx[28], cosx[28];

	if (x < -34 || x > 320 - 80 + 14 || y < -34 || y > 200 + 14) {
		return;
	}


	sin = SinTab[angle];
	cos = CosTab[angle];

	for (i = 0; i < 28; i++) {
		sinx[i] = (i - 14) * sin;
		cosx[i] = (i - 14) * cos;
	}

	ptr = RotBuffer;

	for (i = 0; i < 28; i++) {
		for (j = 0; j < 28; j++, ptr++) {
			fy = i - 14;

			tx = ((cosx[j] - sin * fy) >> 8) + 10;
			ty = ((sinx[j] + cos * fy) >> 8) + 10;

			if (tx >= 0 && tx <= 19 && ty >= 0 && ty <= 19)
				*ptr = spr->Data[tx + (ty << 2) + (ty << 4)];
			else
				*ptr = 0;
		}
	}

	memset(ShadowBuffer, 0, 34 * 34);

	ptr = RotBuffer;
	ptrs = ShadowBuffer; // + SHADOW_XY * 34 + SHADOW_XY;
	for (i = 0; i < 28; i++) {
		for (j = 0; j < 28; j++, ptr++, ptrs++) {
			if (*ptr) *ptrs = SHADOW_COLOR;
		}
		ptrs += (34 - 28);
	}

	ptr = RotBuffer;
	ptrs = ShadowBuffer + SHADOW_XY * 34 + SHADOW_XY;
	for (i = 0; i < 28; i++) {
		for (j = 0; j < 28; j++, ptr++, ptrs++) {
			if (*ptr) *ptrs = *ptr;
		}
		ptrs += (34 - 28);
	}

	PutImage(ShadowBuffer, x - 14 - SHADOW_XY, y - 14 - SHADOW_XY,
		34, 34, safe, 1);
}
