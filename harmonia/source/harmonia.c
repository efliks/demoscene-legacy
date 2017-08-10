/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mem.h>
#include <alloc.h>
#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "harmonia\\typy.c"
#include "harmonia\\prototyp.c"
#include "harmonia\\zegar.c"
#include "harmonia\\grafika.c"
#include "harmonia\\gwiazdy.c"
#include "harmonia\\snd.c"
#include "harmonia\\kapsula.c"
#include "harmonia\\statki.c"
#include "harmonia\\bum.c"
#include "harmonia\\pociski.c"
#include "harmonia\\sterow.c"
#include "harmonia\\laser.c"
#include "harmonia\\intro.c"
#include "harmonia\\font.c"
#include "harmonia\\bonus.c"
#include "harmonia\\wyniki.c"

void main(void)
{
	int i,licznik,pom=0;
	char txt_bufor[4];
	KAPSULA *kaps=&kapsula;

	clrscr();

	printf("Initializing sound...");

	if(!sb_init())
	{
		printf("%s","\nSound Blaster not present!\n\n");
		exit(1);
	}

	printf("%s","OK\nAllocating memory...");

	if((bufor=(unsigned char *)malloc(64000))==NULL ||
	(scroll_bufor=(unsigned char *)malloc(320*66))==NULL ||
	(napis=(unsigned char *)malloc(21120))==NULL)
	{
		free(bufor);
		free(scroll_bufor);
		printf("%s","\nNot enough free memory!\n\n");
		exit(2);
	}

	printf("%s","OK\nLoading files...");

	if(!load_files("sprites.dat",SPRITE_FILES) ||
	!load_files("msprites.dat",MSPRITE_FILES) ||
	!load_files("sounds.dat",SOUND_FILES) ||
	!load_bin(napis,"graphics\\intro.bin"))
	{
		printf("%s%s%s","\nUnable to open file ",missed_file,"\n\n");
		free(bufor);
		free(scroll_bufor);
		free(napis);
		unload_sounds();
		exit(3);
	}

	printf("%s","OK\n");

	odczytaj_rekordy();

	init_font();
	init_keyboard();
	zegar_start();
	tryb_graf();
	paleta_inicjuj();
	save_pal();
	w_inicjuj_sincos();
	randomize();

	sb_dsp_out(ON_SPEAKER);
	enable_dma_int();
	set_freq(SOUND_FREQUENCY);

	while(1)
	{
		intro();

		kaps_inicjuj(kaps);
		p_nadaj0();
		w_nadaj0();
		b_nadaj0(&bonus);
		pole_nadaj0(&pole);
		smuga_nadaj0();
		g_inicjuj();

		ile_aktywnych=0;
		bonus_typ=0;
		licznik=0;

		for(i=0;i<5;i++) stat_inicjuj(&statek[i],kaps);
		pom=0;

		do
		{
			gwiazdy();
			for(i=0;i<10;i++) smuga_wykonaj(&smuga[i]);
			bonus_wykonaj(&bonus,kaps);
			pole_wykonaj(&pole,kaps);
			stat_wykonaj();

			for(i=0;i<30;i++) poc_wykonaj(&pocisk[i]);
			for(i=0;i<10;i++) w_wykonaj(&wybuch[i]);
			kaps_wykonaj(kaps);

			put_string(itoa(kaps->punkty,txt_bufor,10),0,0,bufor);
			put_string(itoa(kaps->giwera+1,txt_bufor,10),312,0,bufor);
			if(bonus_typ)
			{
				if((licznik++)==600) 
				{
					bonus_typ=0;
					licznik=0;
				}
				if(bonus_typ==NOWA_GIW) 
					put_string("Rozbudowa broni",100,0,bufor);
				else if(bonus_typ==BONUS_POLE)
					put_string("Pole silowe",116,0,bufor);
			}
			if(pom<100) pom++;
			
			if(keytab[KEY_SPACE] && !keytab[KEY_RIGHTCTRL] && !keytab[KEY_UPARROW] &&
			!keytab[KEY_DOWNARROW] && !keytab[KEY_LEFTARROW] &&
			!keytab[KEY_RIGHTARROW] && pom==100)
			{
				put_string("Zatrzymano...",98,96,bufor);
				kopiuj_bufor();
				czysc_bufor();
				while(1)
				{
					if(keytab[KEY_RIGHTCTRL] || keytab[KEY_UPARROW] ||
					keytab[KEY_DOWNARROW] || keytab[KEY_LEFTARROW] ||
					keytab[KEY_RIGHTARROW]) break;
				}
			}

			kopiuj_bufor();
			czysc_bufor();
			while(!zegar);
			zegar=0;
			if(keytab[KEY_ESCAPE]) koniec();

		}while(kaps->aktywna);

		fade_down();
		renew_pal();
		analiza_wynikow(kaps->punkty);
	}
}