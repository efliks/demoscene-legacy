/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

#define KEY_SPACE 0x39
#define KEY_ESCAPE 0x01
#define KEY_UPARROW 0x48
#define KEY_DOWNARROW 0x50
#define KEY_LEFTARROW 0x4B
#define KEY_RIGHTARROW 0x4D
#define KEY_RIGHTCTRL 0x1D

#define DOWN 0
#define DOWN_FAST 1
#define DOWN_LEFT 2
#define DOWN_RIGHT 3
#define UP 4
#define UP_FAST 5
#define UP_LEFT 6
#define UP_RIGHT 7
#define SMUGA_N 0
#define SMUGA_T 1

#define BONUS_POLE 1
#define NOWA_GIW 2

#define JOB 0
#define ACID 2
#define BUZZ 4
#define RAIL 6
#define WEAK 8
#define C_LINE 10
#define SPAW 12
#define BALL 14

#define MAX_SPRITES 54
#define MAX_MSPRITES 16
#define MAX_SOUNDS 5
#define SPRITE_FILES 1
#define MSPRITE_FILES 2
#define SOUND_FILES 3

#define MAX_BUM 10

#define SND_EXPLOSION 3
#define SND_BONUS 3
#define SND_ROCKET 4

void tryb_graf(void);
void tryb_text(void);
void kopiuj_bufor(void);
void czysc_bufor(void);
void pixel(int x,int y,unsigned char kolor);
void draw_sprite(unsigned char *sprite,int x,int y,
				int width,int height);
void pasek(int x,int y,unsigned char kolor);
void fade_down(void);
void renew_pal(void);
void save_pal(void);
void retrace(void);
void flash(void);

void g_inicjuj(void);
void gwiazdy(void);

void kaps_inicjuj(KAPSULA *kaps);
void kaps_wykonaj(KAPSULA *kaps);

void smuga_nadaj0(void);
void smuga_inicjuj(int x,int y,int typ,unsigned char kolor);
void smuga_wykonaj(SMUGA *sm);
void p_inicjuj(int x,int y,int typ,int k,int zmiana_xy);
void p_nadaj0(void);
void p_kontrola_kaps(POCISK *poc,KAPSULA *kaps,POLE *ple);
void p_kontrola_stat(POCISK *poc,STATEK *sta,KAPSULA *kaps);
void poc_wykonaj(POCISK *poc);

void stat_inicjuj(STATEK *sta,KAPSULA *kaps);
void stat_ruch(STATEK *sta);
void stat_ruch2(STATEK *statek);
int stat_kstrl(STATEK *sta,KAPSULA *kaps);
void statek_wykonaj(STATEK *sta,int licznik);
void stat_wykonaj(void);
void stat_strzal(STATEK *sta,KAPSULA *kaps);

void laser(int x,int y,KAPSULA *kaps,POLE *ple,int laser_pom);

void przesun_scr_bufor(unsigned char *bufor);
void intro(void);
void koniec(void);
void intro_animuj(void);
void paleta_inicjuj(void);
int load_bin(unsigned char *where,char *filename);
int load_files(unsigned char *listfile,int flag);

void w_inicjuj_sincos(void);
void w_nadaj0(void);
void w_wykonaj(WYBUCH *bum);
void w_inicjuj(int x0,int y0,unsigned char kolor1,unsigned char kolor2);

void put_char(char ascii_code,int x,int y,unsigned char *where);
void put_string(char *text,int x,int y,unsigned char *where);
void init_font(void);

void b_nadaj0(STATEK *bon);
void bonus_inicjuj(STATEK *bon);
void bonus_wykonaj(STATEK *bon,KAPSULA *kaps);
void pole_nadaj0(POLE *p);
void pole_inicjuj(POLE *p);
void pole_wykonaj(POLE *p,KAPSULA *kaps);

int centruj_x(char *string);
void zapisz_rekordy(void);
void odczytaj_rekordy(void);
void zmien_rekordy(int punkty);
void drukuj_liste(void);
void analiza_wynikow(int punkty);
void brak_listy(void);

void init_keyboard(void);
void restore_keyboard(void);

void zegar_start(void);
void zegar_koniec(void);

int sb_init(void);
void sb_dsp_out(unsigned char value);
void interrupt dma_int(void);
void enable_dma_int(void);
void disable_dma_int(void);
void set_freq(unsigned freq);
void set_dma(unsigned char page,unsigned off,unsigned lenght);
void convert_address(unsigned segment,unsigned off1,
					unsigned char *page,unsigned *off2);
void play_sound(SOUND *s);
unsigned get_file_size(char *filename);
int load_sound(SOUND *s,char *filename);
void unload_sound(SOUND *s);
void unload_sounds(void);

unsigned char keytab[256];
int ile_aktywnych;
int bonus_typ;
char missed_file[256];

STATEK bonus;
STATEK statek[5];
POCISK pocisk[30];
KAPSULA kapsula;
POLE pole;
GWIAZDA gwiazda[50];
WYBUCH wybuch[MAX_BUM];
REKORD rekord[12];
SMUGA smuga[10];

int b_punkt[30][60][2];

unsigned char sprite[MAX_SPRITES][400];
unsigned char spr_pocisk[MAX_MSPRITES][64];

SOUND soundtab[MAX_SOUNDS];