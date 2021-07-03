/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

typedef struct typSTATEK
{
	int x,y;
	int kx,ky,zm_x,zm_y;
	int ruch;
	int v;
	int k,klatka[12];
	int aktywny;
	int giwera;
	int xs_min,xs_max;
	int poc_x[3];
	int licznik,pom1,pom2;
	int laser_pom;
}STATEK;

typedef struct typGWIAZDA
{
	int x;
	int y;
	int kolor;
	int v;
}GWIAZDA;

typedef struct typKAPSULA
{
	int x;
	int y;
	int klatka;
	int aktywna;
	int giwera;
	int licznik,l2;
	int punkty;
	int pom,trafiona;
	int pom2;
}KAPSULA;

typedef struct typPOCISK
{
	int x,y;
	int k1,k2;
	int zmiana_xy;
	int aktywny;
	int pom1,pom2,pom3;
}POCISK;

typedef struct typWYBUCH
{
	int x;
	int y;
	int r;
	int kolor1,kolor2;
	int r_pom;
	int aktywny;
}WYBUCH;

typedef struct typPOLE
{
	int aktywne;
	int pom;
	int vpom;
	int r;
	int kolor;
	int czas;
}POLE;

typedef struct typREKORD
{
	int punkty;
	char imie[30];
}REKORD;

typedef struct typSMUGA
{
	int x;
	int y[32];
	int aktywna;
	int typ;
	unsigned char kolor;
}SMUGA;

typedef struct tSOUND
{
	unsigned char *buffer;
	unsigned lenght;
}SOUND;