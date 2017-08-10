

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


int MapIsLoaded = 0;
MAP CurrentMap;

int ScrollSpeed = 1;

char TexErrFile[16];

//------------------------------------------------------------
//------------------------------------------------------------

int LoadMap(char *Filename) {
	
	FILE *p;
	MAPFILEHEADER h;
	MAP *map = &CurrentMap;
	char str[5], file[13];
	int i, j, r, spr_num, k;
	DWORD size;
	FPLAYER fp;
	MAPCELL mc;
	FUNIT fu;


	p = fopen(Filename, "r+b");
	if (!p) {
		return LM_NOFILE;
	}
	fread(&h, sizeof(MAPFILEHEADER), 1, p);
	fclose(p);

	memcpy(str, h.Type, 4);
	str[4] = 0;
	if (strcmp(str, "BRMP") != 0) {
		return LM_FORMATERR;
	}

	if (h.Ver != MAP_VER) {
		return LM_VERERR;
	}

	memcpy(&map->MapInfo, &h.MapInfo, sizeof(MAPINFO));
	strcpy(map->Filename, Filename);
	map->Mx = map->MapInfo.StartX;
	map->My = map->MapInfo.StartY;

//------------------//
// alloc map tables //
//------------------//
	size = map->MapInfo.NumImgFiles * sizeof(SPRITE);
	map->Textures = (TEXTURE *)malloc(size);
	if (!map->Textures) {
		return LM_TEXERR;
	}
	memset(map->Textures, 0, size);

	size = map->MapInfo.SizeX * map->MapInfo.SizeY * sizeof(MAPCELL);
	map->CellData = (MAPCELL *)malloc(size);
	if (!map->CellData) {
		free(map->Textures);
		return LM_CELLERR;
	}
	memset(map->CellData, 0, size);

	/*size = (map->MapInfo.SizeX *
		map->MapInfo.SizeY) * MAP_SMALLCELLS * MAP_SMALLCELLS;
	map->SmallCells = (BYTE *)malloc(size);
	if (!map->SmallCells) {
		free(map->CellData);
		free(map->Textures);
		return LM_SCERR;
	}
	memset(map->SmallCells, 0, size);*/

//-------------//
// alloc units //
//-------------//
	i = AllocUnitTable();
	if (!i) {
		//free(map->SmallCells);
		free(map->CellData);
		free(map->Textures);
		return LM_UNITERR;
	}

//---------------//
// alloc bullets //
//---------------//
	i = AllocBulletTable();
	if (!i) {
		FreeUnitTable();
		//free(map->SmallCells);
		free(map->CellData);
		free(map->Textures);
		return LM_BULLERR;
	}

//------------------//
// alloc explosions //
//------------------//
	i = AllocExplTable();
	if (!i) {
		FreeBulletTable();
		FreeUnitTable();
		//free(map->SmallCells);
		free(map->CellData);
		free(map->Textures);
		return LM_EXPLERR;
	}

//---------------//
// load textures //
//---------------//
	p = fopen(Filename, "r+b");
	fseek(p, h.PtrToImgFiles, SEEK_SET);

	for (i = 0; i < map->MapInfo.NumImgFiles; i++) {
		fread(file, 12, 1, p);
		file[13] = 0;
		
		r = LoadTexture(file, &map->Textures[i]);
		if (r != LT_OK) {
			for (j = 0; j < i; j++) free(map->Textures[j].Data);
			fclose(p);
			FreeExplTable();
			FreeBulletTable();
			FreeUnitTable();
			//free(map->SmallCells);
			free(map->CellData);
			free(map->Textures);
			return r;
		}

	}

//--------------//
// load players //
//--------------//
	fseek(p, h.PtrToPlayers, SEEK_SET);

	memset(PlayerTable, 0, sizeof(PLAYER) * MAX_PLAYERS);

	for (i = 0; i < map->MapInfo.NumPlayers; i++) {
		fread(&fp, sizeof(FPLAYER), 1, p);
		PlayerTable[i].Id = fp.Number;
		PlayerTable[i].Color = fp.Color;
		PlayerTable[i].NumUnits = fp.NumUnits;
		PlayerTable[i].NumBuildings = fp.NumBuildings;
	}

//------------//
// load units //
//------------//
	fseek(p, h.PtrToUnits, SEEK_SET);

	for (i = 0; i < map->MapInfo.NumUnits; i++) {
		fread(&fu, sizeof(FUNIT), 1, p);

		switch (fu.Type) {
			case 1:
				spr_num = SPR_UNIT1;
				break;
			case 2:
				spr_num = SPR_UNIT2;
				break;
			case 3:
				spr_num = SPR_UNIT3;
				break;
			case 4:
				spr_num = SPR_UNIT4;
				break;
			case 5:
				spr_num = SPR_UNIT5;
				break;
			default: // case 6:
				spr_num = SPR_UNIT6;
		}

		InitUnit(&UnitTable[i], fu.PosX, fu.PosY, fu.Type,
			&SpriteData[spr_num], &PlayerTable[fu.Owner]);
	}

//----------------//
// load buildings //
//----------------//
	fseek(p, h.PtrToBuildings, SEEK_SET);

	for (i = 0; i < map->MapInfo.NumBuildings; i++) {
	}

//------------//
// load cells //
//------------//
	fseek(p, h.PtrToMapCells, SEEK_SET);

	fread(map->CellData, sizeof(MAPCELL),
		map->MapInfo.SizeX * map->MapInfo.SizeY, p);

	fclose(p);

//--------------//
// load sprites //
//--------------//
	for (i = 0; i < MAX_SPRITES; i++) {
		j = LoadSprite(&SpriteData[i]);
		if (!j) {
			for (k = 0; k < map->MapInfo.NumImgFiles; k++)
				free(map->Textures[k].Data);
			FreeExplTable();
			FreeBulletTable();
			FreeUnitTable();
			//free(map->SmallCells);
			free(map->CellData);
			free(map->Textures);
			return LM_SPRERR;
		}
	}
	

	return LM_OK;
}

//------------------------------------------------------------
//------------------------------------------------------------

int LoadTexture(char *file, TEXTURE *t) {
	int r;
	char fff[32];

	if (!(t->Data = (BYTE *)malloc(MAP_CELLSIZE * MAP_CELLSIZE))) {
		return LT_MEMERR;
	}

	strcpy(fff, "gfx\\tex\\");
	strcat(fff, file);
	r = LoadBMP(fff, t->Data, 0, MAP_CELLSIZE, MAP_CELLSIZE);
	if (r != 1) {
		strcpy(TexErrFile, file);
		free(t->Data);

		if (r == -1)
			return LT_NOFILE;
		else if (r == -2)
			return LT_FORMATERR;
		else if (r == -3)
			return LT_SIZEERR;
		else //if (r == -4)
			return LT_BITSERR;
	}

	return LT_OK;
}

//------------------------------------------------------------
//------------------------------------------------------------

void FreeMap(void) {
	MAP *map = &CurrentMap;
	int i;

	FreeAllSprites();

	for (i = 0; i < map->MapInfo.NumImgFiles; i++)
		free(map->Textures[i].Data);

	FreeExplTable();
	FreeBulletTable();
	FreeUnitTable();
	//free(map->SmallCells);
	free(map->CellData);
	free(map->Textures);
}

//------------------------------------------------------------

void DrawMap(void) {
	int x, y, cx, ccx, cy, px, ppx, py;
	BYTE *ptr = FrameBuffer;
	MAPCELL *cell;
	MAP *Map = &CurrentMap;

	cx = Map->Mx >> MAP_SHIFT;
	cy = Map->My >> MAP_SHIFT;

	px = Map->Mx & (MAP_CELLSIZE - 1);
	py = Map->My & (MAP_CELLSIZE - 1);

	for (y = 0; y < 200; y++) {

		ccx = cx;
		ppx = px;

		for (x = 0; x < 320 - 80; x++) {
			
			cell = &Map->CellData[ccx + (cy * Map->MapInfo.SizeX)];
			*ptr = Map->Textures[cell->NumOfImg].Data[ppx + (py << MAP_SHIFT)];

			ppx++;
			ppx &= (MAP_CELLSIZE - 1);
			if (!ppx) {
				ccx++;
			}

			ptr++;
		}

		py++;
		py &= (MAP_CELLSIZE - 1);
		if (!py) {
			cy++;
		}

		ptr += 80;
	}
}

//------------------------------------------------------------

void MoveMap(void) {
	int i;

	if (MouseX == 0 || KeyTable[K_LEFTARROW]) {
		for (i = 0; i < ScrollSpeed; i++) {
			if (CurrentMap.Mx > 0)
				CurrentMap.Mx--;
			else
				break;
		}
	}
	else if (MouseX == 319 || KeyTable[K_RIGHTARROW]) {
		for (i = 0; i < ScrollSpeed; i++) {
			if (CurrentMap.Mx < (CurrentMap.MapInfo.SizeX *
				MAP_CELLSIZE - 240 - 2))
				CurrentMap.Mx++;
			else
				break;
		}
	}

	if (MouseY == 0 || KeyTable[K_UPARROW]) {
		for (i = 0; i < ScrollSpeed; i++) {
			if (CurrentMap.My > 0)
				CurrentMap.My--;
			else
				break;
		}
	}
	else if (MouseY == 199 || KeyTable[K_DOWNARROW]) {
		for (i = 0; i < ScrollSpeed; i++) {
			if (CurrentMap.My < (CurrentMap.MapInfo.SizeY *
				MAP_CELLSIZE - 200 - 2))
				CurrentMap.My++;
			else
				break;
		}
	}
}

//------------------------------------------------------------
/*
void ResetSmallCells(void) {
	memset(CurrentMap.SmallCells, 0,
		CurrentMap.MapInfo.SizeX * CurrentMap.MapInfo.SizeY *
		MAP_SMALLCELLS);
}

void ClearSmallCells(void) {
	UNIT *u = UnitTable;
	int i;

	for (i = 0; i < MAX_UNITS; i++, u++) {
		if (!u->status) continue;
		SetSmallCell(u->x, u->y, 0);
	}
}

void SetSmallCell(int x, int y, BYTE val) {
	int tx, ty;
	BYTE *p;

	tx = x / MAP_SMALLCELLSIZE;
	ty = y / MAP_SMALLCELLSIZE;

	p = CurrentMap.SmallCells + tx + ty * CurrentMap.MapInfo.SizeX;

	*p = val;
}

BYTE GetSmallCell(int x, int y) {
	int tx, ty;
	BYTE *p;

	tx = x / MAP_SMALLCELLSIZE;
	ty = y / MAP_SMALLCELLSIZE;

	p = CurrentMap.SmallCells + tx + ty * CurrentMap.MapInfo.SizeX;

	return *p;
}

void SaveSmallCells(void) {
	int x, y;
	BYTE *ptr = CurrentMap.SmallCells, c;
	FILE *p;

	p = fopen("scells.txt", "w");
	if (!p) {
		return;
	}

	for (y = 0; y < CurrentMap.MapInfo.SizeY * MAP_SMALLCELLS; y++) {
		for (x = 0; x < CurrentMap.MapInfo.SizeX * MAP_SMALLCELLS; x++) {
			c = *ptr;
			ptr++;
			c += '0';
			fputc(c, p);
		}

		fputs("\n", p);
	}

	fclose(p);
}
*/
//------------------------------------------------------------

WORD GetCell(int x, int y) {
	return CurrentMap.CellData[(x >> MAP_SHIFT) +
		(y >> MAP_SHIFT) * CurrentMap.MapInfo.SizeX].WhatIs;
}
