#include <dos.h>
#include <mem.h>
#include <alloc.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

#include "graph.c"
#include "font.c"

#define MAXK 40

struct
{
	char string[25];
	int pozycja;
	int v;
}kolumna[MAXK];

void inicjuj_palete(void)
{
	int i;
	outp(0x03c8,0);
	for(i=0;i<55;i++)
	{
		outp(0x03c9,0);
		outp(0x03c9,i);
		outp(0x03c9,i>>1);
	}
	for(i=0;i<9;i++)
	{
		outp(0x03c9,55+i);
		outp(0x03c9,55+i);
		outp(0x03c9,55+i);
	}
}

void wykonaj_kolumny(void)
{
	int i,j;
	unsigned char kolor;
	static licznik;
	for(i=0;i<MAXK;i++)
	{
			for(j=kolumna[i].pozycja-1,kolor=64-8;j>=0;j--)
			{
				if(j<25)
					put_char(kolumna[i].string[j],i<<3,j<<3,kolor,frame_buffer);
				if(kolor>4) kolor-=4;
			}
	}
	for(i=0;i<MAXK;i++)
	{
		switch(kolumna[i].v)
		{
			case 1:
				kolumna[i].pozycja++;
				break;
			case 2:
				if(licznik==1 || licznik==2) kolumna[i].pozycja++;
				break;
			case 3:
				if(licznik==2) kolumna[i].pozycja++;
		}
		if(kolumna[i].pozycja==35)
		{
			for(j=0;j<25;j++) kolumna[i].string[j]=random(64)+32;
			kolumna[i].pozycja=0;
			kolumna[i].v=random(3)+1;
		}
	}
	licznik++;
	if(licznik==3) licznik=0;
}

void inicjuj_kolumny(void)
{
	int i,j;
	for(i=0;i<MAXK;i++)
	{
		for(j=0;j<25;j++) kolumna[i].string[j]=random(255);
		kolumna[i].pozycja=random(25);
		kolumna[i].v=random(3)+1;
	}
}

int main(void)
{
	if((frame_buffer=(unsigned char *)malloc(64000))==NULL) return 1;

	init_font();

	set_vga();
	inicjuj_palete();
	inicjuj_kolumny();

	while(!kbhit())
	{
			wykonaj_kolumny();

			retrace();
			retrace();
			copy_buffer();
			clear_buffer();
	}

	set_text();

	free(frame_buffer);
	return 0;
}