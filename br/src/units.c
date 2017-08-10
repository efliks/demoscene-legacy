
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


UNIT *UnitTable;
BULLET *BulletTable;
EXPL *ExplTable;

int MmapCol0 = MMAP_COL0, MmapCol1 = MMAP_COL1;
int MmapRDir = 1, MmapColR = MMAP_RCOL, MmapRSpd = MMAP_RSPD;

int SelCol = SELECT_COLOR, SelColE = SELECT_COLOR_E, SelMove;

//------------------------------------------------------------

int AllocExplTable(void) {
	ExplTable = (EXPL *)malloc(MAX_EXPL * sizeof(EXPL));
	if (!ExplTable) {
		return 0;
	}

	memset(ExplTable, 0, MAX_EXPL * sizeof(EXPL));

	return 1;
}

void FreeExplTable(void) {
	if (ExplTable) free(ExplTable);
}

void SetExpl(int x, int y) {
	int i, found = 0;
	EXPL *e = ExplTable;

	for (i = 0; i < MAX_EXPL; i++, e++) {
		if (!e->life_timer) {
			found = 1;
			break;
		}
	}

	if (!found) {
		return;
	}

	e->x = x;
	e->y = y;
	e->life_timer = E_LIFETIME;
	e->frame = 0;
	e->f_timer = E_FRAME_TIMER;
	e->spr = &SpriteData[SPR_EXPL];

	for (i = 0; i < E_DOTS; i++) {
		e->r_tab[i] = 0;
		e->rv_tab[i] = random(3) + 1;
	}
}

void ProcessExpl(void) {
	int i, j, k, r, c;
	long x, y;
	EXPL *e = ExplTable;
	SPRITE *spr;
	BYTE *ptr;

	for (i = 0; i < MAX_EXPL; i++, e++) {
		if (!e->life_timer) continue;

		if (e->frame < MAX_EXPLSPR) {
			spr = e->spr + e->frame;

			PutImage(spr->Data, e->x - 10 - CurrentMap.Mx,
				e->y - 10 - CurrentMap.My, 20, 20, 1, 1);

			if (!(--e->f_timer)) {
				e->f_timer = E_FRAME_TIMER;
				e->frame++;
			}
		}

		for (j = 0; j < E_DOTS; j++) {

			e->r_tab[j] += e->rv_tab[j];
			
			r = e->r_tab[j] >> 2;
			
			for (k = 0; k < 2; k++, r -= 6) {
				x = CosTab[j << E_DOTS_SHIFT] * r;
				x >>= SHIFT_CONST;
				y = SinTab[j << E_DOTS_SHIFT] * r;
				y >>= SHIFT_CONST;

				x += (e->x - CurrentMap.Mx);
				y += (e->y - CurrentMap.My);

				if (x < 1 || x > 238 || y < 1 || y > 198) continue;

				c = 255 - r;
				if (c < 192 || c > 255) c = 192;

				ptr = FrameBuffer + x + y * 320;
				*ptr = c;
				*(ptr + 1) = c;
				*(ptr + 320) = c;
				*(ptr + 321) = c;
			}
		}
		
		e->life_timer--;
	}
}

int AllocBulletTable(void) {
	BulletTable = (BULLET *)malloc(MAX_BULLETS * sizeof(BULLET));
	if (!BulletTable) {
		return 0;
	}

	memset(BulletTable, 0, MAX_BULLETS * sizeof(BULLET));

	return 1;
}

void FreeBulletTable(void) {
	if (BulletTable) free(BulletTable);
}

void SetBullet(UNIT *s) {
	int i, found = 0;
	BULLET *b = BulletTable;
	long t, tt;

	for (i = 0; i < MAX_BULLETS; i++, b++) {
		if (!b->life_timer) {
			found = 1;
			break;
		}
	}

	if (!found) {
		return;
	}

	b->tx = s->tx;
	b->ty = s->ty;
	t = s->speed;
	tt = B_SPEED;
	t += tt;
	b->vx = (CosTab[s->a] * t);
	b->vx >>= SHIFT_CONST;
	b->vy = (SinTab[s->a] * t);
	b->vy >>= SHIFT_CONST;
	b->owner = s;
	b->type = s->gun;
	b->spr = &SpriteData[SPR_BULLETS + (b->type << 1)];
	b->f_timer = FRAME_TIMER;
	b->life_timer = B_LIFETIME;
}

void ProcessBullets(void) {
	int i, j, x, y, mx, my, unit_hit;
	BULLET *b = BulletTable;
	SPRITE *cs;
	UNIT *s;
	
	for (i = 0; i < MAX_BULLETS; i++, b++) {
		if (!b->life_timer) continue;

		if ((--b->f_timer) == 0) {
			b->f_timer = FRAME_TIMER;
			b->frame++;
			b->frame &= 1;
		}

		b->tx += b->vx;
		b->ty -= b->vy;

		x = (b->tx >> SHIFT_CONST);
		y = (b->ty >> SHIFT_CONST);

		b->x = x;
		b->y = y;

		mx = x >> MAP_SHIFT;
		my = y >> MAP_SHIFT;

		if (mx < 0 || mx > (CurrentMap.MapInfo.SizeX - 1) ||
			my < 0 || my > (CurrentMap.MapInfo.SizeY - 1)) {
			b->life_timer = 0;
			continue;
		}

		//if (GetWhatIs(mx, my)) {
			unit_hit = 0;
			s = UnitTable;

			for (j = 0; j < CurrentMap.MapInfo.NumUnits; j++, s++) {
				if ((x > s->x - 10) && (x < s->x + 10) &&
					(y > s->y - 10) && (y < s->y + 10) &&
					(s->status != STAT_DEAD)) {
					unit_hit = 1;
					break;
				}
			}

			if (unit_hit) {
				s->status = STAT_DEAD;
				b->life_timer = 0;
				b->owner->kills++;
				SetExpl(s->x, s->y);
				continue;
			}
		//}

		cs = b->spr + b->frame;

		x -= (CurrentMap.Mx + 4);
		y -= (CurrentMap.My + 4);

		PutImage(cs->Data, x, y, 8, 8, 1, 1);

		b->life_timer--;
	}
}

int AllocUnitTable(void) {
	UnitTable = (UNIT *)malloc(MAX_UNITS * sizeof(UNIT));
	if (!UnitTable) {
		return 0;
	}
	
	memset(UnitTable, 0, MAX_UNITS * sizeof(UNIT));

	return 1;
}

void FreeUnitTable(void) {
	if (UnitTable) free(UnitTable);
}

void InitUnit(UNIT *s, int x, int y, int type, SPRITE *spr, PLAYER *p) {
	static int id;

	memset(s, 0, sizeof(UNIT));
	s->id = id++;
	s->type = type;
	s->gun = type;
	s->status = STAT_STOPPED;
	s->x = x;
	s->y = y;
	s->tx = s->x;
	s->tx <<= SHIFT_CONST;
	s->ty = s->y;
	s->ty <<= SHIFT_CONST;
	s->speed = 1;
	s->acc = 2;
	s->spr = spr;
	s->health = 100;
	s->f_timer = FRAME_TIMER;
	s->shoot_lock = SHOOT_TIMER;
	s->owner = p;
}

void ProcessDead(void) {
	int i, x, y;
	UNIT *s = UnitTable;

	for (i = 0; i < CurrentMap.MapInfo.NumUnits; i++, s++) {
			if (s->status == STAT_DEAD) {
			x = s->x - CurrentMap.Mx;
			y = s->y - CurrentMap.My;
			x -= 10;
			y -= 10;
			PutImage(SpriteData[SPR_DEAD1].Data, x, y, 20, 20, 1, 1);
		}
	}
}

void UnitProc(UNIT *s) {
	int x, y, mx, my, chx, chy, new_x, new_y;
	WORD r;

	if (s->status == STAT_DEAD) {
		return;
	}

	


	if (s->status == STAT_MOVING || s->status == STAT_A) {

		chx = chy = 0;

		new_x = s->x;
		new_y = s->y;

		if (s->x < s->dx) {
			if (s->can_move) new_x++;
			chx = 1;
		}
		else if (s->x > s->dx) {
			if (s->can_move) new_x--;
			chx = -1;
		}

		if (s->y < s->dy) {
			if (s->can_move) new_y++;
			chy = 1;
		}
		else if (s->y > s->dy) {
			if (s->can_move) new_y--;
			chy = -1;
		}

		if (!chx && !chy)
			s->status = STAT_STOPPED;
		else {
			if (chx == 1 && chy == 1)
				s->da = MAX_DEGS * 7/8;
			else if (chx == 1 && chy == -1)
				s->da = MAX_DEGS * 1/8;
			else if (chx == -1 && chy == 1)
				s->da = MAX_DEGS * 5/8;
			else if (chx == -1 && chy == -1)
				s->da = MAX_DEGS * 3/8;
			else if (chx == 0 && chy == 1)
				s->da = MAX_DEGS * 6/8;
			else if (chx == 0 && chy == -1)
				s->da = MAX_DEGS * 2/8;
			else if (chx == 1 && chy == 0)
				s->da = MAX_DEGS * 0/8;
			else //if (chx == -1 && chy == 0)
				s->da = MAX_DEGS * 4/8;
		}

		s->can_move = 0;
		s->angle &= (MAX_DEGS - 1);

		if (s->angle < s->da)
			s->angle++;
		else if (s->angle > s->da)
			s->angle--;
		else

		if (s->angle == s->da) {
			s->can_move = 1;
		}
		else if (s->angle < s->da) {
			s->angle++;
		}
		else
			s->angle--;

		/*r = GetCell(new_x, new_y);
		if (r & 1) {
			s->status = STAT_STOPPED;
		}
		else {*/
			s->x = new_x;
			s->y = new_y;
		//}
	}

	
	x = s->x - CurrentMap.Mx;
	y = s->y - CurrentMap.My;

	if ((--s->shoot_lock) == 0) {
		s->shoot_lock = SHOOT_TIMER;
	}


	PutRotSprite(s->spr + s->frame, x, y,
		(s->angle - MAX_DEGS / 4) & (MAX_DEGS - 1), 1);

	if (s->selected) {
		if (s->owner->Id == 0)
			DrawSelection(x, y, 0);
		else
			DrawSelection(x, y, 1);
	}


}

void ProcessUnits(void) {
	int i, n_units = CurrentMap.MapInfo.NumUnits;
	UNIT *unit = UnitTable;

	for (i = 0; i < n_units; i++, unit++) {
		UnitProc(unit);
	}
}

//------------------------------------------------------------

void GetMiniMapCoords(int inx, int iny, int *outx, int *outy) {
	long x, y;

	x = inx;
	x <<= SHIFT_CONST;
	x *= MMAP_W;
	x /= (CurrentMap.MapInfo.SizeX << MAP_SHIFT);
	x >>= SHIFT_CONST;
	*outx = x;

	y = iny;
	y <<= SHIFT_CONST;
	y *= MMAP_H;
	y /= (CurrentMap.MapInfo.SizeY << MAP_SHIFT);
	y >>= SHIFT_CONST;
	*outy = y;
}

void DrawMiniMap(void) {
	UNIT *u;
	int i;
	int x, y, sx, sy;

	GetMiniMapCoords(CurrentMap.Mx, CurrentMap.My, &x, &y);

	GetMiniMapCoords(320 - 80, 200, &sx, &sy);

	x += MMAP_X + 320 - 80;
	y += MMAP_Y;

	DrawFrame(x, y, x + sx, y + sy, MmapCol1);

	u = UnitTable;
	for (i = 0; i < MAX_UNITS; i++, u++) {
		if (!u->status) continue;

		GetMiniMapCoords(u->x, u->y, &x, &y);

		*(FrameBuffer + (x + MMAP_X + 320 - 80) + (y + MMAP_Y) * 320) = MmapCol0;
	}

	DrawRadar();
}

void DrawRadar(void) {
	static int aaa, t, rx, ry;
	int x, y;

	if (!MmapRDir) return;

	if (!t) {
		t = MmapRSpd;
		aaa += MmapRDir;
		aaa &= (MAX_DEGS - 1);
		x = ((SinTab[aaa] * (MMAP_W / 2 - 1)) >> SHIFT_CONST) +
			MMAP_X + 320 - 80 + MMAP_W / 2;
		y = ((CosTab[aaa] * (MMAP_H / 2 - 1)) >> SHIFT_CONST) +
			MMAP_Y + MMAP_H / 2; 
		rx = x;
		ry = y;
	}
	DrawLine(MMAP_X + 320 - 80 + MMAP_W / 2, MMAP_Y + MMAP_W / 2,
			rx, ry, MmapColR);
	t--;
}

//------------------------------------------------------------

void DrawSelection(int ox, int oy, int is_enemy) {
	int i, d = 0;
	long x, y;
	BYTE *b = FrameBuffer, c;

	for (i = 0; i < MAX_DEGS; i++) {
		if ((++d) < 10) continue;
		d = 0;

		x = ((CosTab[(i + SelMove) & (MAX_DEGS - 1)] << 4) >> SHIFT_CONST) + ox;
		y = ((SinTab[(i + SelMove) & (MAX_DEGS - 1)] << 4) >> SHIFT_CONST) + oy;

		if (x < WindowX1 || x > (WindowX2 - 1) ||
			y < WindowY1 || y > (WindowY2 - 1)) continue;

		if (is_enemy)
			c = SelColE;
		else
			c = SelCol;

		*(b + x + (y << 6) + (y << 8)) = c;
	}
}

void AnimateSelections(void) {
	SelMove++;
}

UNIT *GetUnit(void) {
	int i, x, y;
	UNIT *u = UnitTable;

	for (i = 0; i < MAX_UNITS; i++, u++) {
		if (!u->status) continue;

		x = u->x - CurrentMap.Mx;
		y = u->y - CurrentMap.My;

		if (x > (MouseX - 10) && x < (MouseX + 10) &&
			y > (MouseY - 10) && y < (MouseY + 10)) {
			return u;
		}
	}

	return (UNIT *)0;
}

