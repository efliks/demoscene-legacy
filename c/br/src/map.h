
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _map_h
#define _map_h

#define MAP_VER 1

#define MAP_CELLSIZE 64
#define MAP_SHIFT 6 // 2 ^ MAP_SHIFT = MAP_CELLSIZE

#define MAP_SMALLCELLS 2
#define MAP_SMALLCELLSIZE 32

#define IS_UNIT 0x01
#define IS_PLAYER 0x02


#define LM_OK			+1
#define LM_NOFILE		-1
#define LM_FORMATERR	-2
#define LM_VERERR		-3
#define LM_TEXERR		-4
#define LM_CELLERR		-5
#define LM_SCERR		-6
#define LM_UNITERR		-7
#define LM_BULLERR		-8
#define LM_EXPLERR		-9
#define LM_SPRERR		-10

#define LT_OK			+1
#define LT_MEMERR		-100
#define LT_NOFILE		-200
#define LT_FORMATERR	-300
#define LT_SIZEERR		-400
#define LT_BITSERR		-500


//------------------------------------------------------------
//------------------------------------------------------------

typedef struct tMAPINFO {
	WORD	SizeX; // real size = SizeX * MAP_CELLSIZE
	WORD	SizeY;
	WORD	CellSize; // = MAP_CELLSIZE
	WORD	NumPlayers;
	WORD	NumUnits;
	WORD	NumBuildings;
	WORD	NumImgFiles;
	WORD	StartX;
	WORD	StartY;
	char	MapName[32];
	char	BrefingText[512];
	char	NextMapFile[12];
} MAPINFO;

typedef struct tMAPFILEHEADER {
	char	Type[4];
	WORD	Ver;
	MAPINFO	MapInfo;
	DWORD	PtrToPlayers;
	DWORD	PtrToUnits;
	DWORD	PtrToBuildings;
	DWORD	PtrToMapCells;
	DWORD	PtrToImgFiles;
	BYTE	Reserved[8];
} MAPFILEHEADER;

typedef struct tFUNIT {
	WORD	Type;
	WORD	Owner;
	WORD	PosX;
	WORD	PosY;
} FUNIT, FBUILDING;

typedef struct tFPLAYER {
	WORD	Number;
	WORD	Color;
	WORD	NumUnits;
	WORD	NumBuildings;
} FPLAYER;

typedef struct tMAPCELL {
	WORD	NumOfImg;
	WORD	WhatIs;
} MAPCELL;

typedef struct tTEXTURE {
	BYTE	*Data;
} TEXTURE;

typedef struct tMAP {
	MAPINFO	MapInfo;
	TEXTURE	*Textures;
	MAPCELL	*CellData;
	//BYTE	*SmallCells;
	int		Mx, My;
	char	Filename[12];
} MAP;

//------------------------------------------------------------

extern int MapIsLoaded;
extern MAP CurrentMap;

extern int ScrollSpeed;

extern char TexErrFile[16];

//------------------------------------------------------------

int LoadTexture(char *file, TEXTURE *t);
int LoadMap(char *Filename);
void FreeMap(void);
void DrawMap(void);
void MoveMap(void);

/*void ResetSmallCells(void);
void ClearSmallCells(void);
void SetSmallCell(int x, int y, BYTE val);
BYTE GetSmallCell(int x, int y);
void SaveSmallCells(void);*/

WORD GetCell(int x, int y);

#endif
