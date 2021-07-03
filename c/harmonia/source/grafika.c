/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

unsigned char *bufor;
unsigned char paleta_r[256],paleta_g[256],paleta_b[256];

void tryb_graf(void)
{
	/*if((bufor=(unsigned char *)malloc(64000))==NULL) koniec_br();
	memset(bufor,0,64000);*/
	
	asm {
		mov ax,13h
		int 10h }
}

void tryb_text(void)
{
	asm {
		mov ax,03h
		int 10h }

	//free(bufor);
}

void pasek(int x,int y,unsigned char kolor)
{
	asm {
		push es
		les di,bufor
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x
		add di,ax
		mov ah,kolor
		mov al,ah
		stosw
		stosw
		stosw
		stosw
		pop es }
}

void pixel(int x,int y,unsigned char color)
{
	asm {
		mov ax,x
		or ax,ax
		jl quit
		cmp ax,319
		jg quit
		mov bx,y
		or bx,bx
		jl quit
		cmp bx,199
		jg quit

		push es
		les di,bufor
		mov dx,bx
		shl bx,6
		shl dx,8
		add bx,dx
		add bx,ax
		add di,bx
		mov al,color
		stosb
		pop es }
quit:
}

void kopiuj_bufor(void)
{
	asm {
		push ds
		push es
		lds si,bufor
		mov ax,0a000h
		mov es,ax
		xor di,di
		mov cx,16000
		cld
		db 66h
		rep movsw
		pop es
		pop ds }
}

void czysc_bufor(void)
{
	asm {
		push es
		les di,bufor
		xor ax,ax
		mov cx,16000
		cld
		db 66h
		rep stosw
		pop es }
}

void draw_sprite(unsigned char *sprite,int x,int y,
				int width,int height)
{
	asm {
		push ds
		push es
		lds si,sprite
		les di,bufor

		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x
		add di,ax

		mov bx,320
		sub bx,width
		mov dx,height }
ver:
	asm {
		mov cx,width }
hor:
	asm {
		lodsb
		or al,al
		jz next
		mov byte ptr es:[di],al }
next:
	asm {
		inc di
		dec cx
		jnz hor

		add di,bx
		dec dx
		jnz ver

		pop es
		pop ds }	
}

void save_pal(void)
{
	int k;

	outp(0x03c7,0);
	for(k=0;k<256;k++)
	{
		paleta_r[k]=inp(0x03c9);
		paleta_g[k]=inp(0x03c9);
		paleta_b[k]=inp(0x03c9);
	}
}

void renew_pal(void)
{
	int k;

	outp(0x03c8,0);
	for(k=0;k<256;k++)
	{
		outp(0x03c9,paleta_r[k]);
		outp(0x03c9,paleta_g[k]);
		outp(0x03c9,paleta_b[k]);
	}
}

void retrace(void)
{
	asm {
		mov dx,03dah }
retrace1:
	asm {
		in al,dx
		test al,8
		jz retrace1 }
retrace2:
	asm {
		in al,dx
		test al,8
		jnz retrace2 }
}

void flash(void)
{
	int i,j;
	unsigned char r,g,b;

	outp(0x03c8,0);
	for(i=0;i<768;i++) outp(0x03c9,63);

	for(i=0;i<64;i++)
	{
		for(j=0;j<256;j++)
		{
			outp(0x03c7,j);
			r=inp(0x03c9);
			g=inp(0x03c9);
			b=inp(0x03c9);

			if(r>paleta_r[j]) r--;
			if(g>paleta_g[j]) g--;
			if(b>paleta_b[j]) b--;

			outp(0x03c8,j);
			outp(0x03c9,r);
			outp(0x03c9,g);
			outp(0x03c9,b);
		}

		retrace();
	}
}

void fade_down(void)
{
	int i,j;
	unsigned char r,g,b;

	for(j=0;j<64;j++)
	{
		for(i=0;i<256;i++)
		{
			outp(0x03c7,i);
			r=inp(0x03c9);
			g=inp(0x03c9);
			b=inp(0x03c9);
			
			if(r>0) r--;
			if(g>0) g--;
			if(b>0) b--;

			outp(0x03c8,i);
			outp(0x03c9,r);
			outp(0x03c9,g);
			outp(0x03c9,b);
			
		}
		
		retrace();
	}
}