/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void b_nadaj0(STATEK *bon)
{
	bon->aktywny=0;
}

void bonus_inicjuj(STATEK *bon)
{
	int i,j=36;
	bon->x=random(299);
	bon->y=0;
	bon->licznik=0;
	for(i=0;i<6;i++) bon->klatka[i]=j+i;
	bon->aktywny=1;
	bon->k=0;
}

void bonus_wykonaj(STATEK *bon,KAPSULA *kaps)
{
	if(bon->aktywny)
	{
		draw_sprite(sprite[bon->klatka[bon->k]],bon->x,bon->y,20,20);

		if((bon->licznik++)==12) 
		{
			bon->licznik=0;
			bon->k++;
			if(bon->k>5) bon->k=0;
		}
		if((bon->y++)>179) bon->aktywny=0;

		if(bon->x>=kaps->x-10 && bon->x<=kaps->x+30 &&
		bon->y>=kaps->y && bon->y<=kaps->y+20)
		{
			play_sound(&soundtab[SND_BONUS]);

			bon->aktywny=0;
			if(kaps->giwera<3)
			{
				w_inicjuj(kaps->x+10,kaps->y+10,31,32);
				kaps->giwera++;
				bonus_typ=NOWA_GIW;
			}
			else
			{
				pole_inicjuj(&pole);
				bonus_typ=BONUS_POLE;
			}
		}
	}
}

void pole_nadaj0(POLE *p)
{
	p->aktywne=0;
}

void pole_inicjuj(POLE *p)
{
	p->aktywne=1;
	p->pom=0;
	p->vpom=0;
	p->r=10;
	p->kolor=32;
	p->czas=0;
}

void pole_wykonaj(POLE *p,KAPSULA *kaps)
{
	int i,x,y;
	if(p->aktywne)
	{
		if(!p->pom)
		{
			if((p->vpom++)==2)
			{
				p->vpom=0;
				if((p->r++)>15) 
				{
					p->pom=1;
					p->kolor=44;
				}
			}
		}
		else
		{
			if((p->vpom++)==2)
			{
				p->vpom=0;
				if((p->r--)<10)
				{
					p->pom=0;
					p->kolor=32;
				}
			}
		}	
		for(i=1;i<60;i++)
		{
			x=kaps->x+10+b_punkt[p->r][i][0];
			y=kaps->y+10-b_punkt[p->r][i][1];
			pixel(x,y,p->kolor);
		}
		if((p->czas++)==1500) p->aktywne=0;
	}
}