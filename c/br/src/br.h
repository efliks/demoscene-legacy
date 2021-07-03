

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _br_h
#define _br_h

#define MAX_DEGS 256

#define GAME_COLOR 24
#define GAME_ADDCOL 1

#define FRAME_COLOR 15
#define SELECT_COLOR 15
#define SELECT_COLOR_E 210


extern int SinTab[MAX_DEGS], CosTab[MAX_DEGS], TabsOK;
extern BYTE *PanelImg;

extern int ShowMapPos, DumpFormat, GameColor, GameAddCol, ShowDead, IsClock;

extern int FrameIsON, FrameCol, SelectFlag, SelectFlag_II;

//------------------------------------------------------------

void Autostart(void);
void Autoquit(void);

void InitSinCos(void);
int GetCursor(void);

int DoMouse(void);

void DoClock(int Reset);
int Game(void);

#endif
