

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


void interrupt (*OldTimer)(void);
int GlobalCounter = 0, GlobalSec = 0;

//------------------------------------------------------------

void interrupt Timer(void) {
	static int t, tt;

	GlobalCounter++;

	if ((t++) == 91) {
		t = 0;
		GlobalSec++;
	}

	if (((tt++) % 5) == 0) {
		OldTimer();
	}
	else {
		outp(0x20, 0x20);
	}
}

void SetTimer(void) {
	BYTE b;

	b = inp(0x21);
	b |= 1;
	outp(0x21, b);

	OldTimer = getvect(0x08);
	setvect(0x08, Timer);

	b = 0x36; // 001100110
	outp(0x43, b);
	b = 0x38; // 1193180 / 91 = 13112 = 0x3338
	outp(0x40, b);
	b = 0x33;
	outp(0x40, b);

	b = inp(0x21);
	b &= 0xfe;
	outp(0x21, b);
}

void RestoreTimer(void) {
	BYTE b;

	b = inp(0x21);
	b |= 1;
	outp(0x21, b);

	b = 0x36;
	outp(0x43, b);
	b = 0;
	outp(0x40, b);
	outp(0x40, b);

	setvect(0x08, OldTimer);

	b = inp(0x21);
	b &= 0xfe;
	outp(0x21, b);
}
