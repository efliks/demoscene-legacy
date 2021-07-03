

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _kbd_h
#define _kbd_h

#define K_SPACE			0x39
#define K_ESCAPE		0x01
#define K_UPARROW		0x48
#define K_DOWNARROW		0x50
#define K_LEFTARROW		0x4b
#define K_RIGHTARROW	0x4d
#define K_RIGHTCTRL		0x1d
#define K_F9			0x43
#define K_F10			0x44
#define K_COMMA			0x33
#define K_PERIOD		0x34
#define K_TILDE			0x29

//------------------------------------------------------------

extern BYTE KeyTable[256];

//------------------------------------------------------------

void SetKeyboard(void);
void RestoreKeyboard(void);

#endif
