

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _mouse_h
#define _mouse_h

#define MOUSE_COLOR 30

#define MAX_MOUSESPR 13
#define MOUSE_TIMER 25

#define MSPR_BUILD 0
#define MSPR_DOWN 2
#define MSPR_LEFT 3
#define MSPR_RIGHT 4
#define MSPR_UP 5
#define MSPR_DEF 6
#define MSPR_DEST 7
#define MSPR_SELECT1 9
#define MSPR_SELECT2 11

//------------------------------------------------------------

extern int MouseX, MouseY, MouseB;
extern int MouseOK, MouseSpeed, MouseSprOK;

//------------------------------------------------------------

int InitMouse(void);
void ReadMouse(void);
void DrawMouse(int num_spr);
void SetMouseSpeed(void);
int LoadMouseSpr(void);
void FreeMouseSpr(void);

#endif
