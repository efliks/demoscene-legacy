

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "progs/br/newtypes.h"
#include "progs/br/map.h"

//------------------------------------------------------------
//------------------------------------------------------------

#define MAP_VER 1

#define MAP_CELLSIZE 64
#define MAP_SHIFT 6 // 2 ^ MAP_SHIFT = MAP_CELLSIZE

//------------------------------------------------------------

#define N_PLAYERS 1
#define N_UNITS 10
#define N_BUILDINGS 1
#define N_TEXTURES 16
#define SIZE_X 30
#define SIZE_Y 30
#define START_X 10
#define START_Y 10
#define N_TYPES 5

//------------------------------------------------------------
//------------------------------------------------------------

int IsInt(char *str) {
	int i = 0;
	char c;

	if (!str[0]) return 0;

	while (1) {
		c = str[i++];
		if (c == 0) break;
		
		if (((c >= '0') && (c <= '9')) || ((i == 1) && (c == '-')))
			continue;
		else
			return 0;
	}

	return 1;
}

//------------------------------------------------------------

int main(int argc, char **argv) {

	FILE *p;
	MAPFILEHEADER mfh;
	MAPINFO mi;
	FUNIT fu;
	FPLAYER fp;
	FBUILDING fb;
	MAPCELL mc;
	int i, j;
	char str1[16], str2[32], str[12];
	int size_x, size_y, n_units, n_players, *ii;
	time_t t;
	char tstr[32];

	if (argc < 5) {
		printf("%s\n", "syntax: mkmap size_x size_y n_units n_players tex_str");
		return -1;
	}

	/*ii = &size_x;
	for (i = 0; i < 4; i++) {
		if (!IsInt(argv[i + 1])) {
			printf("%s\n", "unable to convert str->int");
			return -2;
		}
		*ii = atoi(argv[i + 1]);
		ii++;
	}*/

	size_x = atoi(argv[1]);
	size_y = atoi(argv[2]);
	n_units = atoi(argv[3]);
	n_players = atoi(argv[4]);

	if (strlen(argv[5]) > 3) {
		printf("%s\n", "tex_str error");
		return -3;
	}

	if (size_x > 50 || size_x < 20) {
		printf("%s\n", "size_x error");
		return -4;
	}

	if (size_y > 50 || size_y < 20) {
		printf("%s\n", "size_y error");
		printf("%d\n", size_y);
		return -5;
	}

	if (n_units > 100 || n_units < 2) {
		printf("%s\n", "n_units error");
		return -6;
	}

	if (n_players < 2 || n_players > 4) {
		printf("%s\n", "n_players error");
		return -7;
	}

	randomize();

	p = fopen("mapxx.map", "w+b");
	if (!p) return 1;

	memset(&mfh, 0, sizeof(MAPFILEHEADER));

	mfh.Type[0] = 'B';
	mfh.Type[1] = 'R';
	mfh.Type[2] = 'M';
	mfh.Type[3] = 'P';
	mfh.Ver = MAP_VER;
	mfh.MapInfo.SizeX = size_x;
	mfh.MapInfo.SizeY = size_y;
	mfh.MapInfo.CellSize = MAP_CELLSIZE;
	mfh.MapInfo.NumPlayers = n_players;
	mfh.MapInfo.NumUnits = n_units;
	mfh.MapInfo.NumBuildings = N_BUILDINGS;
	mfh.MapInfo.NumImgFiles = N_TEXTURES;
	mfh.MapInfo.StartX = START_X;
	mfh.MapInfo.StartY = START_Y;
	t = time(NULL);
	ltoa(t, tstr, 10);
	strcpy(mfh.MapInfo.MapName, tstr);
	//strcpy(mfh.MapInfo.MapName, "TEST MAP");
	strcpy(mfh.MapInfo.BrefingText, "THIS IS A BRIEFING");
	strcpy(mfh.MapInfo.NextMapFile, "map01.map");
	mfh.PtrToPlayers = sizeof(MAPFILEHEADER) + 0;
	mfh.PtrToUnits = mfh.PtrToPlayers + sizeof(FPLAYER) * n_players;
	mfh.PtrToBuildings = mfh.PtrToUnits + sizeof(FUNIT) * n_units;
	mfh.PtrToMapCells = mfh.PtrToBuildings + sizeof(FBUILDING) * N_BUILDINGS;
	mfh.PtrToImgFiles = mfh.PtrToMapCells + sizeof(MAPCELL) * size_x * size_y;
	memset(mfh.Reserved, 0, 8);

	fwrite(&mfh, sizeof(MAPFILEHEADER), 1, p);

	for (i = 0; i < n_players; i++) {
		fp.Number = i;
		fp.Color = i;
		if (!i) {
			fp.NumUnits = n_units;
			fp.NumBuildings = N_BUILDINGS;
		}
		else {
			fp.NumUnits = 0;
			fp.NumBuildings = 0;
		}

		fwrite(&fp, sizeof(FPLAYER), 1, p);
	}

	for (i = 0; i < n_units; i++) {
		fu.Type = random(N_TYPES);
		fu.Owner = i & 1;
		fu.PosX = random((size_x * MAP_CELLSIZE) - 60) + 30;
		fu.PosY = random((size_y * MAP_CELLSIZE) - 60) + 30;
		fwrite(&fu, sizeof(FUNIT), 1, p);
	}

	for (i = 0; i < N_BUILDINGS; i++) {
		fb.Type = 0;
		fb.Owner = 0;
		fb.PosX = random((size_x * MAP_CELLSIZE) - 60) + 30;
		fb.PosY = random((size_y * MAP_CELLSIZE) - 60) + 30;
		fwrite(&fb, sizeof(FBUILDING), 1, p);
	}

	for (i = 0; i < size_x * size_y; i++) {
		mc.NumOfImg = random(N_TEXTURES);

		if (mc.NumOfImg == 15)
			mc.WhatIs = 1;
		else
			mc.WhatIs = 0;


		fwrite(&mc, sizeof(MAPCELL), 1, p);
	}

	for (i = 0; i < N_TEXTURES; i++) {
		itoa(i, str1, 10);
		strcpy(str2, "tex");
		strcat(str2, argv[5]);
		//if (i < 100) {
		//	strcat(str2, "0");
			if (i < 10) {
				strcat(str2, "0");
			}
		//}

		
		strcat(str2, str1);
		strcat(str2, ".bmp");

		for (j = 0; j < 12; j++) str[j] = 0;
		for (j = 0; j < strlen(str2); j++) str[j] = str2[j];
		fwrite(str, 12, 1, p);
	}

	fclose(p);

	return 0;
}