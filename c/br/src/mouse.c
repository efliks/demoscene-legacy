

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


BYTE imgMouse[12 * 12] = {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
						1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
						1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
						1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

SPRITE sprMouse[MAX_MOUSESPR] = {{"mouse/build1.bmp", 12, 12, 0},
	{"mouse/build2.bmp", 12, 12, 0},
	{"mouse/cdown.bmp", 12, 12, 0},
	{"mouse/cleft.bmp", 12, 12, 0},
	{"mouse/cright.bmp", 12, 12, 0},
	{"mouse/cup.bmp", 12, 12, 0},
	{"mouse/cursor.bmp", 12, 12, 0},
	{"mouse/desta.bmp", 12, 12, 0},
	{"mouse/destb.bmp", 12, 12, 0},
	{"mouse/select1a.bmp", 12, 12, 0},
	{"mouse/select1b.bmp", 12, 12, 0},
	{"mouse/select2a.bmp", 12, 12, 0},
	{"mouse/select2b.bmp", 12, 12, 0}};

int MouseX, MouseY, MouseB;
int MouseOK = 0, MouseSpeed = 10, MouseSprOK = 0;

//------------------------------------------------------------

int InitMouse(void) {
	int r = 0, i;

	_AX = 0x00;
	geninterrupt(0x33);

	if (_AX == 0xffff) {
		r = 1;

		_AX = 0x04;
		_CX = 160;
		_DX = 100;
		geninterrupt(0x33);

		for (i = 0; i < 12 * 12; i++) {
			if (imgMouse[i]) imgMouse[i] = MOUSE_COLOR;
		}

		SetMouseSpeed();

		MouseB = 0;
		MouseOK = 1;
	}
	
	return r;
}

void ReadMouse(void) {
	int x, y;

	_AX = 0x03;
	geninterrupt(0x33);

	MouseX = _CX >> 1;
	MouseY = _DX;
	MouseB = _BX;
}

void DrawMouse(int num_spr) {
	int x = MouseX, y = MouseY, f;
	static int counter, frame;
	
	SetWindow(0, 0, 320, 200);

	if (MouseSprOK) {
		f = num_spr;

		if (num_spr != MSPR_DEF) {
			x -= 6;
			y -= 6;
		}
		
		if (num_spr == MSPR_BUILD || num_spr == MSPR_DEST ||
			num_spr == MSPR_SELECT1 || num_spr == MSPR_SELECT2) {
			f += frame;
		}

		PutImage(sprMouse[f].Data, x, y, 12, 12, 1, 1);
	}
	else
		PutImage(imgMouse, x, y, 12, 12, 1, 1);
	
	SetWindow(0, 0, 240, 200);

	if ((++counter) == MOUSE_TIMER) {
		counter = 0;

		frame++;
		frame &= 1;
	}
}

void SetMouseSpeed(void) {
	_AX = 0x00f;
	_CX = MouseSpeed + 5;
	_DX = MouseSpeed + 5;
	geninterrupt(0x33);
}

int LoadMouseSpr(void) {
	int i, t;
	SPRITE *s = sprMouse;

	for (i = 0; i < MAX_MOUSESPR; i++, s++) {
		t = LoadSprite(&sprMouse[i]);
		if (t != 1) {
			FreeMouseSpr();
			return 0;
		}
	}

	MouseSprOK = 1;
	return 1;
}

void FreeMouseSpr(void) {
	int i;
	SPRITE *s = sprMouse;

	for (i = 0; i < MAX_MOUSESPR; i++, s++) {
		if (s->Data) free(s->Data);
	}

	MouseSprOK = 0;
}
