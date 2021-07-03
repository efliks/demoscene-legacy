/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

int zegar=0;
void interrupt (*stary_zegar)(void);

void interrupt nowy_zegar(void)
{
	zegar++;
	zegar&=7;
}

void zegar_start(void)
{
	stary_zegar=getvect(0x1c);
	setvect(0x1c,nowy_zegar);
	asm {
		mov al,110100b
		out 43h,al
		xor ax,ax
		out 40h,al
		mov al,0x20
		out 40h,al }
}

void zegar_koniec(void)
{
	setvect(0x1c,stary_zegar);
	asm {
		mov al,110100b
		out 43h,al
		mov al,-1
		out 40h,al
		out 40h,al }
}