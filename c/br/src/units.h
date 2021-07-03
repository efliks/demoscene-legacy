

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _units_h
#define _units_h

#define SHIFT_CONST 8

#define MAX_SPEED 3 << SHIFT_CONST

#define RADAR_SPEED 2

#define FRAME_TIMER 16

#define MMAP_X 10
#define MMAP_Y 10
#define MMAP_W 60
#define MMAP_H 60

#define MMAP_COL0 50
#define MMAP_COL1 15

#define MMAP_RCOL 15
#define MMAP_RSPD 3


#define STAT_DEAD 1
#define STAT_STOPPED 2
#define STAT_MOVING 3
#define STAT_MOVING1 4
#define STAT_A 5

#define MAX_BULLETS 100
#define MAX_UNITS 200

#define B_LIFETIME 150
#define B_SPEED 1 << SHIFT_CONST

#define SHOOT_TIMER 20

#define MAX_EXPL 10
#define E_FRAME_TIMER 20
#define E_LIFETIME (E_FRAME_TIMER * MAX_EXPLSPR) + 50
#define E_DOTS MAX_DEGS / 8
#define E_DOTS_SHIFT 3 // E_DOTS = MAX_DEGS / 2^E_DOTS_SHIFT

//------------------------------------------------------------

typedef struct tUNIT {
	int		id;
	int		x, y;
	long	tx, ty;
	long	vx, vy;
	long	speed;
	long	acc;
	int		angle, a;
	int		type;
	int		status;
	int		kills;
	int		gun;
	int		health;
	SPRITE	*spr;
	int		frame;
	int		f_timer;
	int		m;
	int		shoot_lock;
	PLAYER	*owner;
	int		selected;
	int		dx, dy, da, can_move;
} UNIT;

typedef struct tBULLET {
	int		x, y;
	long	tx, ty;
	long	vx, vy;
	int		frame;
	int		f_timer;
	int		type;
	int		life_timer;
	UNIT	*owner;
	SPRITE	*spr;
} BULLET;

typedef struct tEXPL {
	int		x, y;
	int		frame;
	int		f_timer;
	int		life_timer;
	SPRITE	*spr;
	int		r_tab[E_DOTS];
	int		rv_tab[E_DOTS];
} EXPL;

//------------------------------------------------------------

extern UNIT *UnitTable, PlayerUnit;
extern BULLET *BulletTable;
extern EXPL *ExplTable;

extern int MmapCol0, MmapCol1;
extern MmapRDir, MmapRSpd, MmapColR;

extern int SelCol, SelColE, SelMove;

//------------------------------------------------------------

int AllocExplTable(void);
void FreeExplTable(void);
void SetExpl(int x, int y);
void ProcessExpl(void);
int AllocBulletTable(void);
void FreeBulletTable(void);
void SetBullet(UNIT *s);
void ProcessBullets(void);
int AllocUnitTable(void);
void FreeUnitTable(void);
void InitUnit(UNIT *s, int x, int y, int type, SPRITE *spr, PLAYER *p);
void UnitProc(UNIT *s);
void ProcessUnits(void);
void ProcessDead(void);

void GetMiniMapCoords(int inx, int iny, int *outx, int *outy);
void DrawMiniMap(void);
void DrawRadar(void);

void DrawSelection(int ox, int oy, int is_enemy);
void AnimateSelections(void);

UNIT *GetUnit(void);

#endif
