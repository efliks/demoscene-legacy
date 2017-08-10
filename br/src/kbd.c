

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


void interrupt (*OldKbdInt)(void);
BYTE KeyTable[256];

//------------------------------------------------------------

void interrupt KbdInt(void) {
	BYTE c;

	c = inportb(0x60);
	outportb(0x20, 0x20);

	if (c & 0x80)
		KeyTable[c - 0x80] = 0;
	else
		KeyTable[c] = 1;
}

void SetKeyboard(void) {
	int i;

	for (i = 0; i < 256; i++) {
		KeyTable[i] = 0;
	}

	OldKbdInt = getvect(0x09);

	setvect(0x09, KbdInt);
}

void RestoreKeyboard(void) {
	setvect(0x09, OldKbdInt);
}
