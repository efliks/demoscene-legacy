/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void smuga_nadaj0(void)
{
	int i;
	SMUGA *sm;

	for(i=0;i<10;i++)
	{
		sm=&smuga[i];
		sm->aktywna=0;
	}
}

void smuga_inicjuj(int x,int y,int typ,unsigned char kolor)
{
	int i;
	SMUGA *sm;

	for(i=0;i<10;i++)
	{
		sm=&smuga[i];

		if(!sm->aktywna)
		{
			sm->x=x;
			sm->y[0]=y;
			sm->typ=typ;
			sm->kolor=kolor;
			sm->aktywna=1;
			break;
		}
	}
}

void smuga_wykonaj(SMUGA *sm)
{
	int i;
	unsigned char kolor=sm->kolor;

	if(sm->aktywna)
	{
		for(i=31;i>0;i--) sm->y[i]=sm->y[i-1];
		for(i=0;i<32;i++,kolor-=2)
		{
			if(sm->y[i]>=0 && sm->y[i]<=199)
				pasek(sm->x,sm->y[i],kolor);
		}
		if(sm->typ==UP_FAST)
			sm->y[0]-=2;
		else
			sm->y[0]+=2;
		if(sm->y[0]<0-31 || sm->y[0]>199+31)
			sm->aktywna=0;
	}
}

void p_inicjuj(int x,int y,int typ,int k,int zmiana_xy)
{
	int i,st,kon;
	POCISK *poc;

	if(zmiana_xy<4)
	{
		st=4;
		kon=30;
	}
	else
	{
		st=0;
		kon=4;
	}

	for(i=st;i<kon;i++)
	{
		poc=&pocisk[i];

		if(!poc->aktywny)
		{
			if(typ==SMUGA_T) 
			{
				if(zmiana_xy==DOWN_FAST)
					smuga_inicjuj(x,y,zmiana_xy,163+63);
				else if(zmiana_xy==UP_FAST)
					smuga_inicjuj(x,y+8,zmiana_xy,163);
			}

			poc->x=x;
			poc->y=y;
			poc->k1=k;
			poc->k2=poc->k1+1;
			poc->zmiana_xy=zmiana_xy;
			poc->pom1=0;
			poc->pom2=0;
			poc->pom3=0;
			poc->aktywny=1;
			break;
		}
	}
}

void p_nadaj0(void)
{
	int i;
	POCISK *poc;

	for(i=0;i<30;i++)
	{
		poc=&pocisk[i];
		poc->aktywny=0;
	}
}

void p_kontrola_kaps(POCISK *poc,KAPSULA *kaps,POLE *ple)
{
	if(poc->x>kaps->x && poc->x<kaps->x+19 &&
	poc->y>kaps->y && poc->y<kaps->y+19 && 
	!kaps->trafiona && !ple->aktywne)
	{
		play_sound(&soundtab[random(SND_EXPLOSION)]);

		kaps->giwera--;
		kaps->trafiona=1;
		if(kaps->giwera<0) 
		{
			kaps->aktywna=0;
			kaps->klatka=42;
		}
		else
			w_inicjuj(kaps->x+10,kaps->y+10,12,44);
	}
}

void p_kontrola_stat(POCISK *poc,STATEK *sta,KAPSULA *kaps)
{
	if(poc->x>sta->x && poc->x<sta->x+19 &&
	poc->y>sta->y && poc->y<sta->y+19 && sta->aktywny==1)
	{
		sta->aktywny=0;
		sta->licznik=0;
		sta->k=6;
		w_inicjuj(sta->x+10,sta->y+10,12,44);
		kaps->punkty++;
		kaps->licznik++;
		ile_aktywnych--;

		play_sound(&soundtab[random(SND_EXPLOSION)]);
	}
}

void poc_wykonaj(POCISK *poc)
{
	int i;
	if(poc->aktywny)
	{
		if(poc->pom1==0)
			draw_sprite(spr_pocisk[poc->k1],poc->x,poc->y,8,8);
		else
			draw_sprite(spr_pocisk[poc->k2],poc->x,poc->y,8,8);
		if((poc->pom2++)==10)
		{
			poc->pom2=0;
			if(poc->pom1==0) 
				poc->pom1=1;
			else
				poc->pom1=0;
		}
		switch(poc->zmiana_xy)
		{
			case DOWN:
				poc->y++;
				p_kontrola_kaps(poc,&kapsula,&pole);
				if(poc->y>191) poc->aktywny=0;
				break;
			case DOWN_FAST:
				poc->y+=2;
				p_kontrola_kaps(poc,&kapsula,&pole);
				if(poc->y>191) poc->aktywny=0;
				break;
			case DOWN_LEFT:
				poc->y++;
				poc->pom3++;
				if(poc->pom3==2)
				{
					poc->pom3=0;
					poc->x--;
				}
				p_kontrola_kaps(poc,&kapsula,&pole);
				if(poc->y>191 || poc->x<0) poc->aktywny=0;
				break;
			case DOWN_RIGHT:
				poc->y++;
				poc->pom3++;
				if(poc->pom3==2)
				{
					poc->pom3=0;
					poc->x++;
				}
				p_kontrola_kaps(poc,&kapsula,&pole);
				if(poc->y>191 || poc->x>311) poc->aktywny=0;
				break;
			case UP:
				poc->y--;
				for(i=0;i<5;i++) 
					p_kontrola_stat(poc,&statek[i],&kapsula);
				if(poc->y<0) poc->aktywny=0;
				break;
			case UP_FAST:
				poc->y-=2;
				for(i=0;i<5;i++) 
					p_kontrola_stat(poc,&statek[i],&kapsula);
				if(poc->y<0) poc->aktywny=0;
				break;
			case UP_LEFT:
				poc->y--;
				poc->pom3++;
				if(poc->pom3==2)
				{
					poc->pom3=0;
					poc->x--;
				}
				for(i=0;i<5;i++) 
					p_kontrola_stat(poc,&statek[i],&kapsula);
				if(poc->y<0 || poc->x<0) poc->aktywny=0;
				break;
			case UP_RIGHT:
				poc->y--;
				poc->pom3++;
				if(poc->pom3==2)
				{
					poc->pom3=0;
					poc->x++;
				}
				for(i=0;i<5;i++) 
					p_kontrola_stat(poc,&statek[i],&kapsula);
				if(poc->y<0 || poc->x>311) poc->aktywny=0;
		}
	}
}