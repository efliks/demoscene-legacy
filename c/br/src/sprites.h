

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _sprites_h
#define _sprites_h

#define MAX_SPRITES	65
#define SPRITES_DIR	"gfx/"

#define SPR_UNIT1	0
#define SPR_UNIT2	6
#define SPR_UNIT3	12
#define SPR_UNIT4	18
#define SPR_UNIT5	24
#define SPR_UNIT6	30
#define SPR_UNIT7	36
#define SPR_DEAD1	42
#define SPR_BULLETS	43
#define SPR_EXPL	59

#define SPR_PER_UNIT 6

#define MAX_MAPSPR	16
#define MAX_BULSPR	16
#define MAX_EXPLSPR	6

#define SHADOW_COLOR 192
#define SHADOW_XY 4

//------------------------------------------------------------

typedef struct tSPRITE {
	char Filename[24];
	WORD Width, Height;
	BYTE *Data;
} SPRITE;

//------------------------------------------------------------

extern int LoadedSprites;
extern char SprErrMsg[128];


int LoadSprite(SPRITE *Spr);
void FreeAllSprites(void);
void PutRotSprite(SPRITE *spr, int x, int y, int angle, int safe);

#endif
