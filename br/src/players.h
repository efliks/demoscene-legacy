

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _players_h
#define _players_h

#define MAX_PLAYERS 8


typedef struct tPLAYER {
	BYTE	Id;
	BYTE	Color;
	WORD	Killed;
	WORD	NumUnits, NumBuildings;
} PLAYER;

//------------------------------------------------------------

extern PLAYER PlayerTable[MAX_PLAYERS];

//------------------------------------------------------------

#endif
