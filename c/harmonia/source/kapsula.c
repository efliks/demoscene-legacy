/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void kaps_inicjuj(KAPSULA *kaps)
{
	kaps->x=149;
	kaps->y=179;
	kaps->klatka=48;
	kaps->aktywna=1;
	kaps->giwera=0;
	kaps->punkty=0;
	kaps->licznik=0;
	kaps->l2=0;
	kaps->pom=0;
	kaps->pom2=0;
	kaps->trafiona=0;
}

void kaps_wykonaj(KAPSULA *kaps)
{
	draw_sprite(sprite[kaps->klatka],kaps->x,kaps->y,20,20);
	
	if((kaps->pom2++)==12)
	{
		kaps->pom2=0;
		if(kaps->klatka==48)
			kaps->klatka=49;
		else
			kaps->klatka=48;
	}
	if(kaps->licznik==20)
	{
		bonus_inicjuj(&bonus);
		kaps->licznik=0;
	}
	if(kaps->trafiona)
	{
		if((kaps->pom++)==200)
		{
			kaps->pom=0;
			kaps->trafiona=0;
		}
	}
	if(keytab[KEY_RIGHTARROW])
	{
		if(kaps->x<299) kaps->x++;
	}
	if(keytab[KEY_LEFTARROW])
	{
		if(kaps->x>0) kaps->x--;
	}
	if(keytab[KEY_UPARROW])
	{
		if(kaps->y>128) kaps->y--;
	}
	if(keytab[KEY_DOWNARROW])
	{
		if(kaps->y<178) kaps->y++;
	}
	if(keytab[KEY_RIGHTCTRL] && (kaps->l2++)==13)
	{
		kaps->l2=0;
		switch(kaps->giwera)
		{
			case 0:
				p_inicjuj(kaps->x+6,kaps->y-1,SMUGA_N,WEAK,UP);
				break;
			case 1:
				p_inicjuj(kaps->x,kaps->y-1,SMUGA_N,C_LINE,UP);
				p_inicjuj(kaps->x+12,kaps->y-1,SMUGA_N,C_LINE,UP);
				break;
			case 2:
				p_inicjuj(kaps->x,kaps->y-1,SMUGA_N,SPAW,UP_FAST);
				p_inicjuj(kaps->x+12,kaps->y-1,SMUGA_N,SPAW,UP_FAST);
				break;
			case 3:
				p_inicjuj(kaps->x,kaps->y-9,SMUGA_T,BALL,UP_FAST);
				p_inicjuj(kaps->x+12,kaps->y-9,SMUGA_T,BALL,UP_FAST);
				p_inicjuj(kaps->x,kaps->y-9,SMUGA_N,BALL,UP_LEFT);
				p_inicjuj(kaps->x+12,kaps->y-9,SMUGA_N,BALL,UP_RIGHT);
		}
	}				
}