

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <mem.h>
#include <dos.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "progs/br/newtypes.h"
#include "progs/br/br.h"
#include "progs/br/timer.h"
#include "progs/br/mouse.h"
#include "progs/br/vga.h"
#include "progs/br/font.h"
#include "progs/br/console.h"
#include "progs/br/sprites.h"
#include "progs/br/players.h"
#include "progs/br/map.h"
#include "progs/br/units.h"
#include "progs/br/bmpfiles.h"
#include "progs/br/pcxfiles.h"
#include "progs/br/kbd.h"

#include "progs/br/timer.c"
#include "progs/br/mouse.c"
#include "progs/br/vga.c"
#include "progs/br/font.c"
#include "progs/br/console.c"
#include "progs/br/sprites.c"
#include "progs/br/players.c"
#include "progs/br/map.c"
#include "progs/br/units.c"
#include "progs/br/bmpfiles.c"
#include "progs/br/pcxfiles.c"
#include "progs/br/kbd.c"

//------------------------------------------------------------

int SinTab[MAX_DEGS], CosTab[MAX_DEGS], TabsOK = 0;
BYTE *PanelImg;

int ShowMapPos = 0, DumpFormat = 1,
	GameColor = GAME_COLOR, GameAddCol = GAME_ADDCOL, ShowDead = 0,
	IsClock = 0;

int FrameIsON = 0, FrameCol = FRAME_COLOR;
int SelectFlag = 0, SelectFlag_II = 0;

//------------------------------------------------------------

void Autostart(void) {
	int i;
	char str[42];

	ParseCommand("SET V_LOGFLAG 1");
	
	SetTimer();
	for (i = 0; i < 3; i++) {
		Print(".");
		Show();
		GlobalSec = 0;
		while (GlobalSec < 1);
	}
	Print("TIMER SET TO 91,0 HZ");
	Show();

	InitSinCos();
	Print("LOOKUP TABLES SET");
	Show();
	
	randomize();
	Print("RANDOM NUMBER GENERATOR SET");
	Show();

	i = LoadPalette("gfx/palette.bin", MainPalette);
	if (!i) {
		Print("UNABLE TO LOAD PALETTE");
	}
	else {
		SetPalette(MainPalette);
		Print("PALETTE LOADED");
	}
	Show();

	i = LoadConsoleImg("gfx/console.bmp");
	if (i < 0) {
		Print("UNABLE TO LOAD CONSOLE IMAGE");
	}
	else {
		Print("CONSOLE IMAGE LOADED");
	}
	Show();

	PanelImg = (BYTE *)malloc(80 * 200);
	if (!PanelImg) {
		Print("UNABLE TO ALLOC PANEL");
	}

	i = LoadBMP("gfx/panel.bmp", PanelImg, 0, 80, 200);
	if (!i) {
		memset(PanelImg, 0, 80 * 200);
		Print("UNABLE TO LOAD PANEL");
	}
	else {
		Print("PANEL LOADED");
	}
	Show();

	i = InitMouse();
	if (!i) {
		Print("UNABLE TO INIT MOUSE");
	}
	else {
		Print("MOUSE OK");
	}
	Show();

	i = LoadMouseSpr();
	if (!i) {
		for (i = 0; i < strlen(SprErrMsg); i++) {
			if (i == 39) break;
			str[i] = toupper(SprErrMsg[i]);
		}
		str[i] = 0;
		Print(str);

		Print("UNABLE TO LOAD MOUSE SPRITE");
	}
	else {
		Print("MOUSE SPRITES LOADED");
	}

	Print("LOADING AUTOEXEC.CFG...");
	Show();

	LoadCfg();
}

void Autoquit(void) {
	if (MapIsLoaded) {
		ParseCommand("FREEMAP");
		Show();
	}

	if (ConImgFlag) {
		FreeConsoleImg();
		Print("CONSOLE IMAGE FREED");
		Show();
	}

	if (PanelImg) {
		free(PanelImg);
		Print("PANEL IMAGE FREED");
		Show();
	}

	if (MouseSprOK) {
		FreeMouseSpr();
		Print("MOUSE SPRITES FREED");
		Show();
	}

	RestoreTimer();
	Print("TIMER RESTORED");
}

//------------------------------------------------------------

void InitSinCos(void) {
	int i;
	float a = 0.0;

	for (i = 0; i < MAX_DEGS; i++) {
		SinTab[i] = (int)(sin(a) * 256.0);
		CosTab[i] = (int)(cos(a) * 256.0);

		a += (M_PI / (MAX_DEGS / 2));
	}

	TabsOK = 1;
}

//------------------------------------------------------------

int DoMouse(void) {
	static x1, y1, x2, y2, fx1, fy1, fx2, fy2, gx, gy;
	int cur = MSPR_DEF, i, x, y;
	UNIT *u;
	long mx, my;

	if (MouseX > 239) {
		
		if (MouseB == 1 &&
			MouseX >= (320 - 80 + MMAP_X) &&
			MouseX < (320 - 80 + MMAP_X + MMAP_W) &&
			MouseY >= MMAP_Y && MouseY < (MMAP_Y + MMAP_H)) {

			mx = MouseX - (320 - 80) - MMAP_X;
			mx <<= SHIFT_CONST;
			mx *= ((CurrentMap.MapInfo.SizeX << MAP_SHIFT));
			mx /= MMAP_W;
			mx >>= SHIFT_CONST;
			
			my = MouseY - MMAP_Y;
			my <<= SHIFT_CONST;
			my *= ((CurrentMap.MapInfo.SizeY << MAP_SHIFT));
			my /= MMAP_H;
			my >>= SHIFT_CONST;
			
			if (mx > ((CurrentMap.MapInfo.SizeX << MAP_SHIFT) - 240))
				mx = ((CurrentMap.MapInfo.SizeX << MAP_SHIFT) - 240 - 1);

			if (my > ((CurrentMap.MapInfo.SizeY << MAP_SHIFT) - 200))
				my = ((CurrentMap.MapInfo.SizeY << MAP_SHIFT) - 200 - 1);

			CurrentMap.Mx = mx;
			CurrentMap.My = my;
		}

		cur = MSPR_DEF;
	}
	else {
		if (MouseB == 1) {

			if (SelectFlag) {

				if (MouseX != gx || MouseY != gy) {

					u = UnitTable;
					for (i = 0; i < MAX_UNITS; i++, u++) {
						if (u->selected && u->owner->Id == 0) {
							u->dx = MouseX + CurrentMap.Mx;
							u->dy = MouseY + CurrentMap.My;
							u->status = STAT_MOVING;
						}
					}
				}

				return MSPR_DEST;
			}

			if (!FrameIsON) {

				gx = MouseX;
				gy = MouseY;

				u = GetUnit();
				if (u) {
					u->selected = 1;
					
					if (u->owner->Id == 0) {
						SelectFlag = 1;
						cur = MSPR_DEST;
					}
					else
						SelectFlag_II = 1;
				}
			}

			if (!SelectFlag) {
				
				if (!FrameIsON) {
					x1 = MouseX;
					y1 = MouseY;
					FrameIsON = 1;
				}
				else {
					x2 = MouseX;
					y2 = MouseY;

					fx1 = x1;
					fy1 = y1;
					fx2 = x2;
					fy2 = y2;

					if (fx1 > fx2) {
						i = fx1;
						fx1 = fx2;
						fx2 = i;
					}
					
					if (fy1 > fy2) {
						i = fy1;
						fy1 = fy2;
						fy2 = i;
					}

					if (fx1 != fx2 && fy1 != fy2) {
						DrawFrame(fx1, fy1, fx2, fy2, FrameCol);
					}
				}

				cur = MSPR_DEF;
			}
		}
		else if (MouseB == 2) {

			if (SelectFlag || SelectFlag_II) {
				u = UnitTable;
				for (i = 0; i < MAX_UNITS; i++, u++) {
					if (!u->status) continue;
					u->selected = 0;
				}

				SelectFlag = SelectFlag_II = 0;
			}

			cur = MSPR_DEF;
		}
		else {
			if (FrameIsON) {
				gx = MouseX;
				gy = MouseY;
				u = UnitTable;
				for (i = 0; i < MAX_UNITS; i++, u++) {
					if (!u->status) continue;

					x = u->x - CurrentMap.Mx;
					y = u->y - CurrentMap.My;

					if (x > fx1 && x < fx2 &&
						y > fy1 && y < fy2) {

						u->selected = 1;

						if (u->owner->Id == 0)
							SelectFlag = 1;
						else
							SelectFlag_II = 1;
					}
				}

				FrameIsON = 0;

				if (SelectFlag) cur = MSPR_DEST;
			}
			else {
				if (!SelectFlag) {
					u = UnitTable;
					for (i = 0; i < MAX_UNITS; i++, u++) {
						if (!u->status) continue;

						x = u->x - CurrentMap.Mx;
						y = u->y - CurrentMap.My;

						if (x > (MouseX - 10) && x < (MouseX + 10) &&
							y > (MouseY - 10) && y < (MouseY + 10)) {

							if (u->owner->Id == 0)
								cur = MSPR_SELECT1;
							else
								cur = MSPR_SELECT2;

							break;
						}
					}

				}
				else
					cur = MSPR_DEST;
			}

		}
	}

	return cur;
}

//------------------------------------------------------------

void DoClock(int Reset) {
	static int PrevSec, Hr, Min, Sec;
	char Txt1[32], Txt2[4];
	
	if (Reset) {
		Hr = Min = Sec = 0;
	}

	if (PrevSec != GlobalSec) {
		Sec++;
		if (Sec == 60) {
			Sec = 0;
			Min++;
			if (Min == 60) {
				Min = 0;
				Hr++;
				if (Hr == 24) {
					Hr = 0;
				}
			}
		}

		PrevSec = GlobalSec;
	}

	if (IsClock) {
		strcpy(Txt1, "");
		if (Hr < 10) {
			strcat(Txt1, "0");
		}
		itoa(Hr, Txt2, 10);
		strcat(Txt1, Txt2);
		strcat(Txt1, ":");
		if (Min < 10) {
			strcat(Txt1, "0");
		}
		itoa(Min, Txt2, 10);
		strcat(Txt1, Txt2);
		strcat(Txt1, ":");
		if (Sec < 10) {
			strcat(Txt1, "0");
		}
		itoa(Sec, Txt2, 10);
		strcat(Txt1, Txt2);

		PutString(Txt1, 0, 0, ConColor, ConAddCol);
	}
}

//------------------------------------------------------------

int Game(void) {
	char c;
	int r, n_loops = 0, cur, CanDump = 0, cdump = 0;
	char ScrStr[30], tmp[8];


	SetKeyboard();


	while (1) {

		PutImage(PanelImg, 320 - 80, 0, 80, 200, 0, 0);

		DrawMap();

		DrawMiniMap();

		if (ShowDead) ProcessDead();	

		ProcessUnits();
		
		ProcessBullets();

		ProcessExpl();

		ReadMouse();

		cur = DoMouse();

		if (!FrameIsON) MoveMap();
	
		DrawMouse(cur);

		DoClock(0);

		AnimateSelections();



		if (ShowMapPos) {
			itoa(CurrentMap.Mx, ScrStr, 10);
			strcat(ScrStr, ", ");
			itoa(CurrentMap.My, tmp, 10);
			strcat(ScrStr, tmp);
			PutString(ScrStr, 0, 200 - 8, ConColor, ConAddCol);
		}


		if (KeyTable[K_ESCAPE] || KeyTable[K_TILDE]) {
			FadeDown();
			break;
		}

		if (KeyTable[K_F9]) {
			if (CanDump) {
				ParseCommand("DUMP");
				CanDump = 0;
			}

			cdump++;
			if (cdump == 50) {
				cdump = 0;
				CanDump++;
				CanDump &= 1;
			}
		}
		else
			CanDump = 1;

		//Retrace();

		Wait(1);

		CopyFrameBuffer();

		if (!(n_loops++)) FadeUp();
	}

	RestoreKeyboard();

	FadeDown();

	return r;
}

//------------------------------------------------------------

int main(void) {
	int i;

	i = SetFrameBuffer();
	if (!i) {
		printf("%s\n", "unable to set frame buffer");
		return 1;
	}

	i = SetConsole();
	if (!i) {
		FreeFrameBuffer();
		printf("%s\n", "unable to set console");
		return 2;
	}

	SetVGA();

	Autostart();

	while (1) {
		if (!Console()) break;

		if (!TabsOK || !MapIsLoaded || !MouseOK) {
			Print("UNABLE TO START GAME");
			continue;
		}
		else
			i = Game();

		//if (!i) break;
	}

	FadeDown();

	Autoquit();

	SetText();

	FreeConsole();

	FreeFrameBuffer();
	
	return 0;
}
