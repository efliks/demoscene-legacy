/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void laser(int x,int y,KAPSULA *kaps,POLE *ple,int laser_pom)
{
	while(y<199-8)
	{
		if(laser_pom)
		{	
			draw_sprite(spr_pocisk[BUZZ],x,y,8,8);
			laser_pom=0;
		}
		else
		{
			draw_sprite(spr_pocisk[BUZZ+1],x,y,8,8);
			laser_pom=1;
		}
		y+=8;
	}

	if(x>=kaps->x && x<=kaps->x+20 &&
	kaps->aktywna && !kaps->trafiona && !ple->aktywne)
	{
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