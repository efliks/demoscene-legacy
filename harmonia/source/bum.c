/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

#define PI 3.14159265359

void w_inicjuj_sincos(void)
{
	int i,r;

	for(r=10;r<40;r++)
	{	
		for(i=0;i<360;i+=6)
		{
			b_punkt[r-10][i/6][0]=(int)(cos(i*PI/180)*r);
			b_punkt[r-10][i/6][1]=(int)(sin(i*PI/180)*r);
		}
	}
}

void w_nadaj0(void)
{
	int i;
	WYBUCH *b;

	for(i=0;i<MAX_BUM;i++)
	{
		b=&wybuch[i];
		b->aktywny=0;
	}
}

void w_inicjuj(int x0,int y0,
				unsigned char kolor1,unsigned char kolor2)
{
	int i;
	WYBUCH *b;

	for(i=0;i<10;i++)
	{
		b=&wybuch[i];

		if(!b->aktywny)
		{
			b->x=x0;
			b->y=y0;
			b->r=3;
			b->kolor1=kolor1;
			b->kolor2=kolor2;
			b->r_pom=0;
			b->aktywny=1;
			break;
		}
	}
}

void w_wykonaj(WYBUCH *bum)
{
	int j,x1,y1,x2,y2;

	if(bum->aktywny)
	{
		for(j=1;j<60;j++)
		{
			x1=bum->x+b_punkt[bum->r][j][0];
			y1=bum->y-b_punkt[bum->r][j][1];
			pixel(x1,y1,bum->kolor1);

			x2=bum->x+b_punkt[bum->r-2][j][0];
			y2=bum->y-b_punkt[bum->r-2][j][1];
			pixel(x2,y2,bum->kolor2);
		}
		if((bum->r_pom++)==2)
		{
			bum->r++;
			bum->r_pom=0;
		}
		if(bum->r>28) bum->aktywny=0;
	}
}