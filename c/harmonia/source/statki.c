/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void stat_inicjuj(STATEK *sta,KAPSULA *kaps)
{
	int i,j;
	if(!random(2))
		sta->x=random(30);
	else
		sta->x=random(30)+269;
	sta->y=random(79);
	sta->zm_x=0;
	sta->zm_y=0;
	sta->aktywny=1;
	sta->xs_min=random(319);
	sta->xs_max=sta->xs_min+random(319-sta->xs_min);
	for(i=0;i<3;i++) sta->poc_x[i]=random(319);
	sta->licznik=0;
	sta->k=0;
	sta->laser_pom=0;
	if(kaps->punkty<20)
		sta->giwera=0;
	else if(kaps->punkty>=20 && kaps->punkty<40)
		sta->giwera=random(2);
	else if(kaps->punkty>=40 && kaps->punkty<60)
		sta->giwera=random(3);
	else if(kaps->punkty>=60 && kaps->punkty<80)
		sta->giwera=random(4);
	else if(kaps->punkty>=80 && kaps->punkty<100)
		sta->giwera=random(5);
	else
		sta->giwera=random(6);
	if(sta->giwera!=5) 
	{
		sta->ruch=random(2);
		sta->v=random(2);
	}
	else 
	{
		sta->ruch=2;
		sta->v=1;
	}
	j=(sta->giwera)*6;
	for(i=0;i<6;i++) sta->klatka[i]=j+i;
	for(i=6,j=42;i<12;i++,j++) sta->klatka[i]=j;
	ile_aktywnych++;
}

void stat_ruch(STATEK *sta)
{
	if(!sta->ruch)
	{
		if((sta->x++)>=299) sta->ruch=1;
	}	
	else if(sta->ruch==1)
	{
		if((sta->x--)<1) sta->ruch=0;
	}
	else stat_ruch2(sta);
}

int stat_kstrl(STATEK *sta,KAPSULA *kaps)
{
	int zwrot=0,i;
	if(sta->giwera==0 || sta->giwera==1)
	{
		if(sta->x>kaps->x-30 && sta->x<kaps->x+60) zwrot=1;
	}
	else if(sta->giwera==2)
	{
		for(i=0;i<3;i++)
		{
			if(sta->x>sta->poc_x[i]-30 && sta->x<sta->poc_x[i]+60) zwrot=1;
		}
	}
	else
	{
		if(sta->x>=sta->xs_min && sta->x<=sta->xs_max) zwrot=1;
	}
	return zwrot;
}

void statek_wykonaj(STATEK *sta,int licznik)
{
	if(sta->aktywny==1)
	{ 		
		draw_sprite(sprite[sta->klatka[sta->k]],sta->x,sta->y,20,20);
		
		// animacja
		if(stat_kstrl(sta,&kapsula))
		{
			sta->pom1=5;
			sta->pom2=4;
		}
		else
		{
			sta->pom1=3;
			sta->pom2=0;
		}
		if((sta->licznik++)==12)
		{
			if(sta->laser_pom)
				sta->laser_pom=0;
			else
				sta->laser_pom=1;
			sta->licznik=0;
			sta->k++;
			if(sta->k>sta->pom1) sta->k=sta->pom2;
		}
		
		stat_strzal(sta,&kapsula);
		switch(sta->v)
		{
			case 0:
				stat_ruch(sta); 
				break;
			case 1:
				if(licznik==2 || licznik==3) stat_ruch(sta);
		}
	}
	else if(!sta->aktywny)
	{
		draw_sprite(sprite[sta->klatka[sta->k]],sta->x,sta->y,20,20);
		
		if((sta->licznik++)==25)
		{
			sta->licznik=0;
			sta->k++;
			if(sta->k>11) sta->aktywny=-1;
		}
	}
	else
	{
		sta->licznik++;
		if(sta->licznik==200) stat_inicjuj(sta,&kapsula);
	}
}

void stat_wykonaj(void)
{
	static int licznik;
	int i;

	if((licznik++)>2) licznik=0;
	for(i=0;i<5;i++) statek_wykonaj(&statek[i],licznik);
}

void stat_strzal(STATEK *sta,KAPSULA *kaps)
{
	int i,nr;
	switch(sta->giwera)
	{
		case 0:
			if(sta->x==kaps->x)
				p_inicjuj(sta->x+6,sta->y+20,SMUGA_N,JOB,DOWN);
			break;
		case 1:
			if(sta->x==kaps->x)
			{
				p_inicjuj(sta->x,sta->y+20,SMUGA_N,JOB,DOWN);
				p_inicjuj(sta->x+12,sta->y+20,SMUGA_N,JOB,DOWN);
			}
			break;
		case 2:
			for(i=0;i<3;i++)
			{
				if(sta->x==sta->poc_x[i])
				{
					p_inicjuj(sta->x,sta->y+20,SMUGA_N,ACID,DOWN_LEFT);
					p_inicjuj(sta->x+12,sta->y+20,SMUGA_T,ACID,DOWN_RIGHT);
				}
			}
			break;
		case 3:
			if(sta->x>=sta->xs_min && sta->x<=sta->xs_max)
				laser(sta->x+6,sta->y+20,&kapsula,&pole,sta->laser_pom);
			break;
		case 4:
			if(sta->x>=sta->xs_min && sta->x<=sta->xs_max)
			{
				laser(sta->x,sta->y+20,&kapsula,&pole,sta->laser_pom);
				laser(sta->x+12,sta->y+20,&kapsula,&pole,sta->laser_pom);
			}
			break;
		case 5:
			if(sta->x==kaps->x)
			{
				p_inicjuj(sta->x,sta->y+20,SMUGA_T,RAIL,DOWN_FAST);
				p_inicjuj(sta->x+12,sta->y+20,SMUGA_T,RAIL,DOWN_FAST);

				play_sound(&soundtab[SND_ROCKET]);
			}
	}			
}

void stat_ruch2(STATEK *statek)
{
	if(statek->ruch)
	{
		if(statek->zm_x)
		{
			if(statek->x<statek->kx)
			{
				if((statek->x++)==statek->kx) statek->zm_x=0;
			}
			else
			{
				if((statek->x--)==statek->kx) statek->zm_x=0;
			}
		}
		if(statek->zm_y)
		{
			if(statek->y<statek->ky)
			{
				if((statek->y++)==statek->ky) statek->zm_y=0;
			}
			else
			{
				if((statek->y--)==statek->ky) statek->zm_y=0;
			}			
		}
		if(!statek->zm_x && !statek->zm_y)
		{
			statek->kx=random(299);
			statek->ky=random(100);
			statek->zm_x=1;
			statek->zm_y=1;
		}
	}
}